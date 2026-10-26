/*
 * version.c  真・三國無双7 Empires(日本語版 Steam、Launch.exe 1.0.5.0)用 mod
 *
 * ゲームのフォルダ(Launch.exe と同じ場所)に version.dll として置くと、ゲームの起動時に読み込まれ、
 * CE で作って実機確認した次の改造を自動で入れる(設定は同じ場所の version.ini)。
 *   1. 横取りパッチ v4          (steal_patch_v4_three_tier.CEA)
 *   2. 随伴の制御 v5            (escort_control_v5.CEA)と 味方の野良雑魚兵の有無
 *   3. 固有武将の表示上限       (unique_model_boost_v5.CEA)
 *   4. 読み込みの表の守り       (load_table_guard_v1.CEA)
 *   5. クラッシュの守り         (crash_guard_v3.CEA)と、DLL版で追加した守り(動きのデータ・範囲外の番号・付け替え先なし など)
 *   6. HPの倍率                 (TypeIDごと、version.ini の [HpScale] 区画。Mode=1 受けるダメージを減らす / Mode=2 最大HPを増やす)
 *   7. 真ん中の帯の縦の位置     (「〜を撃破せよ!」など、version.ini の [Message] BannerY)
 *   8. 会話の窓の縦の位置       (左の顔の絵・名前・セリフ、version.ini の [Message] TalkOffsetX / TalkOffsetY)
 *   9. 地図の切り替えに「非表示」を足す(全体 → 周辺 → 非表示、version.ini の [Map] ToggleHidden)
 *  10. 周辺地図の位置と大きさ   (version.ini の [Map] SurroundX / SurroundY / SurroundSize)
 *  11. 相性のスローの速さ       (敵の攻撃・反撃の演出のスロー、version.ini の [Affinity] SlowSpeed)
 *  12. 反撃の猶予の有無         (相性負けの時の、敵の攻撃の出始めの反撃の猶予、version.ini の [Affinity] CounterWindow)
 * 割り込みの命令列は hooks.S(CEA と同じ命令。Launch.exe の番地だけを実行時に埋める)。
 *
 * 安全装置:
 *   - Launch.exe の「作成日時の印」(PE の TimeDateStamp = 0x56CEA82C、日本語版 1.0.5.0)が違えば何もしない。
 *   - 書き換える場所ごとに、元の命令のバイト列が CEA の assert と同じかを確かめ、違えばその機能だけ入れない。
 *   - 書き換える間は他のスレッドを止め、書き換える場所を実行中でないことを確かめる。
 *   - 結果は同じフォルダの mod_log.txt に、動いた回数などは mod_status.txt(30秒ごと)に書く。
 *
 * ビルド(MSYS2 の「MINGW32」、またはLinuxの i686-w64-mingw32-gcc):
 *   gcc -O2 -shared -s -static-libgcc -o version.dll version.c hooks.S version.def
 * dinput8.dll という名前に変えて置いても動く(その時は本物の dinput8.dll へ中継する)。
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define MOD_VERSION "2026-11-04 (18)"
#define JP_TIMESTAMP 0x56CEA82Cu

/* ================================================================
 *  本物の DLL への中継(version.dll / dinput8.dll)
 * ================================================================ */
#define PROXY_LIST(X) \
    X(GetFileVersionInfoA) X(GetFileVersionInfoByHandle) X(GetFileVersionInfoExA) X(GetFileVersionInfoExW) \
    X(GetFileVersionInfoSizeA) X(GetFileVersionInfoSizeExA) X(GetFileVersionInfoSizeExW) X(GetFileVersionInfoSizeW) \
    X(GetFileVersionInfoW) X(VerFindFileA) X(VerFindFileW) X(VerInstallFileA) X(VerInstallFileW) \
    X(VerLanguageNameA) X(VerLanguageNameW) X(VerQueryValueA) X(VerQueryValueW) \
    X(DirectInput8Create) X(DllCanUnloadNow) X(DllGetClassObject) X(DllRegisterServer) X(DllUnregisterServer)

#define DECL_PFN(n) FARPROC pfn_##n;
PROXY_LIST(DECL_PFN)

static HMODULE g_self, g_real;
static char g_dir[MAX_PATH];
static CRITICAL_SECTION g_proxyLock;
static volatile LONG g_proxyResolved;

/* 中継の入口から最初に呼ばれた時に本物を読み込む(DllMain の中では LoadLibrary しない) */
void __cdecl resolve_proxy(void)
{
    if (g_proxyResolved) return;
    EnterCriticalSection(&g_proxyLock);
    if (!g_proxyResolved) {
        char self[MAX_PATH], sys[MAX_PATH], path[MAX_PATH];
        const char *base;
        GetModuleFileNameA(g_self, self, MAX_PATH);
        base = strrchr(self, '\\');
        base = base ? base + 1 : self;
        GetSystemDirectoryA(sys, MAX_PATH);
        snprintf(path, sizeof(path), "%s\\%s", sys, _stricmp(base, "dinput8.dll") == 0 ? "dinput8.dll" : "version.dll");
        g_real = LoadLibraryA(path);
        if (g_real) {
#define RESOLVE(n) pfn_##n = GetProcAddress(g_real, #n);
            PROXY_LIST(RESOLVE)
        }
        InterlockedExchange(&g_proxyResolved, 1);
    }
    LeaveCriticalSection(&g_proxyLock);
}

/* 中継の入口: 本物の関数へそのまま飛ぶ(引数・戻り値・スタックに触らない) */
#define PROXY_STUB(n) \
    "  .globl _proxy_" #n "\n" \
    "_proxy_" #n ":\n" \
    "  cmp dword ptr [_pfn_" #n "],0\n" \
    "  jne 1f\n" \
    "  pushad\n" \
    "  call _resolve_proxy\n" \
    "  popad\n" \
    "1:\n" \
    "  jmp dword ptr [_pfn_" #n "]\n"
__asm__(".intel_syntax noprefix\n .text\n" PROXY_LIST(PROXY_STUB) ".att_syntax\n");

/* ================================================================
 *  hooks.S の記号
 * ================================================================ */
extern DWORD g_base, g_pIsBadReadPtr, g_lref_table[];
extern DWORD R_steal, R_ecReg, R_ecMember, R_ecBudgetSkip, R_ecBudgetMember, R_umb, R_ltWait, R_cgDiv, R_cgDraw, R_cgPart, R_cgFx;
extern char hk_steal[], hk_ecReg[], hk_ecMember[], hk_ecBudgetSkip[], hk_ecBudgetMember[], hk_umb[], hk_ltWait[], hk_ltFull[],
            hk_cgDiv[], hk_cgDraw[], hk_cgPart[], hk_cgFx[], hk_cgBadId[], hk_cgCharRec[],
            hk_cgRedir1[], hk_cgRedir2[], hk_cgRedir3[], hk_cgRow[], hk_hpScale[], hk_hpDmg[], hk_mapToggle[], hk_slowCap[];
/* 設定 */
extern DWORD stThreshold, pbToA, pbToB, pbToN, ltMinFree;
extern BYTE ecMasterEnabled, ecAllyEscortEnabled, ecEnemyEscortEnabled, ecRewriteEnabled,
            ecBaseRewriteEnabled, ecRoamRewriteEnabled, ecMemberRewriteEnabled, ecBudgetFilterEnabled;
extern DWORD ecRewriteListCount, ecRewriteList[8], ecBaseRewriteListCount, ecBaseRewriteList[8],
             ecRoamRewriteListCount, ecRoamRewriteList[8], ecEnemyLinkedPercent;
/* 記録 */
extern DWORD hookHitCount, fullCount, allyFullCount, stealSuccessCount, stealFailCount;
extern char cg_motionExitA[], cg_motionExitB[];
extern DWORD cgRowSkipCount, R_cgRow;
extern DWORD R_mapToggle, mapModeCount, mapToggleCount, mapLastMode;
extern DWORD R_slowCap, slowCapCount;
extern float slowMin, slowLastV;
extern float hpScaleTable[256], hpDmgTable[256];
extern float msgBandY, msgBandY2, msgFxY, msgFxY2;
extern double msgTalkY1, msgTalkY2;
extern float msgTalkX, msgFrameY1, msgFrameY2;
extern double msgFrameX;
extern BYTE hpBodyOnly;
extern DWORD hpScaledCount, hpLastTypeId, R_hpScale, hpDmgCount, hpDmgLastTypeId, R_hpDmg;
extern DWORD cgRedirSkipCount, cgRedirLastWhere, R_cgRedir1, R_cgRedir2, R_cgRedir3;
extern DWORD cgIdSkipCount, cgIdLastId, cgIdLastCaller, cgRecSkipCount, cgRecLastId, R_cgCharRec;
extern DWORD pbCount, ltLastFree, ltRejectCount, ltFullCount, cgSkipCount, cgDrawSkipCount, cgDrawBadPtr, cgDrawBadWho, cgFxSkipCount;
extern DWORD ecHookFireCount, ecBlockedCount, ecLinkedBlockedCount, ecRewrittenCount, ecMemberRewrittenCount,
             ecBudgetSkipCount, ecBudgetMemberClearedCount, ecBaseRewrittenCount, ecRoamRewrittenCount;

/* ================================================================
 *  記録ファイル
 * ================================================================ */
static void mod_log(const char *fmt, ...)
{
    char path[MAX_PATH], buf[1024];
    SYSTEMTIME t;
    va_list ap;
    FILE *f;
    snprintf(path, sizeof(path), "%s\\mod_log.txt", g_dir);
    f = fopen(path, "ab");
    if (!f) return;
    GetLocalTime(&t);
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    fprintf(f, "%04d-%02d-%02d %02d:%02d:%02d  %s\r\n", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, buf);
    fclose(f);
}

/* ================================================================
 *  設定(version.ini)
 * ================================================================ */
typedef struct {
    int stealPatch, stealThreshold;
    int allyStrayZako, allyEscort, enemyEscort, enemyEscortRewrite, enemyLinkedZakoLevel;
    int baseEscortRewrite, roamingEscortRewrite;
    DWORD squad[8], baseSquad[8], roamSquad[8];
    int nSquad, nBaseSquad, nRoamSquad;
    int uniqueModelBoxes;
    int loadTableGuard, loadTableGuardMinFree;
    int hpScaleCount, hpDmgCount;
    double bannerY, talkOffsetY, talkOffsetX;
    int mapToggleHidden;
    double miniX, miniY, miniSize;
    double slowSpeed;
    int counterWindow;
    int crashGuardDivZero, crashGuardBadDraw, crashGuardNullFx, crashGuardMotion, crashGuardBadId, crashGuardRedirect;
} Config;
static Config cfg;
static char g_ini[MAX_PATH];

static int ini_int(const char *key, int def)
{
    return (int)GetPrivateProfileIntA("Mod", key, def, g_ini);
}

/* 奪わないID(護衛任務に必要な14種)。旅団長 2060〜2064 は書き換え先として使ってよい。 */
static int is_forbidden_owner(DWORD v)
{
    static const DWORD prot[] = {2055, 2071, 2075, 2076, 2078, 2079, 2080, 2083, 2084};
    size_t i;
    for (i = 0; i < sizeof(prot) / sizeof(prot[0]); i++) if (v == prot[i]) return 1;
    if (v == 2308 || v == 2309) return 2;   /* プレイヤー用 */
    return 0;
}

/* "2065,2070" や "[2065, 2070]" を読む。不正なら -1 */
static int ini_list(const char *key, DWORD out[8])
{
    char s[512];
    const char *p;
    int n = 0;
    GetPrivateProfileStringA("Mod", key, "", s, sizeof(s), g_ini);
    for (p = s; *p;) {
        if (*p >= '0' && *p <= '9') {
            unsigned long long v = 0;
            while (*p >= '0' && *p <= '9') { v = v * 10 + (unsigned)(*p - '0'); if (v > 0x7FFFFFFF) break; p++; }
            if (v > 0x7FFFFFFF) { mod_log("[エラー] %s: 値が大きすぎます。", key); return -1; }
            if (n >= 8) { mod_log("[エラー] %s: 候補は最大8件までです。", key); return -1; }
            if (is_forbidden_owner((DWORD)v) == 1) { mod_log("[エラー] %s: %u は奪わないIDのため指定できません。", key, (unsigned)v); return -1; }
            if (is_forbidden_owner((DWORD)v) == 2) { mod_log("[エラー] %s: %u はプレイヤー用のIDのため指定できません。", key, (unsigned)v); return -1; }
            out[n++] = (DWORD)v;
        } else if (*p == '-') {
            mod_log("[エラー] %s: 0以上の整数で指定してください。", key); return -1;
        } else p++;
    }
    return n;
}

/* [HpScale] 区画: 上から順に読む。"Mode=1"(受けるダメージを減らす、既定)/"Mode=2"(最大HPを増やす)の行で
   方式を切り替え、その下の "TypeID=倍率" / "最初-最後=倍率" をその方式の表に入れる。同じ方式で同じ TypeID は後の行が優先。
   BodyOnly=1/0 は両方の方式に共通。 */
static void load_hp_scale(void)
{
    static char buf[8192];
    char *p;
    int mode = 1;
    DWORD n = GetPrivateProfileSectionA("HpScale", buf, sizeof(buf), g_ini);
    hpBodyOnly = 1;
    cfg.hpScaleCount = 0;
    cfg.hpDmgCount = 0;
    for (p = buf; n && *p; p += strlen(p) + 1) {
        char key[64], *eq = strchr(p, '='), *dash, *end;
        int a, b, t;
        double v;
        size_t kl;
        if (!eq || *p == ';') continue;
        kl = (size_t)(eq - p);
        if (kl >= sizeof(key)) continue;
        memcpy(key, p, kl); key[kl] = 0;
        while (kl && (key[kl - 1] == ' ' || key[kl - 1] == '\t')) key[--kl] = 0;
        if (_stricmp(key, "BodyOnly") == 0) { hpBodyOnly = atoi(eq + 1) ? 1 : 0; continue; }
        if (_stricmp(key, "Mode") == 0) {
            int m = atoi(eq + 1);
            if (m == 1 || m == 2) mode = m;
            else mod_log("[エラー] [HpScale] %s: Mode は 1(受けるダメージを減らす)か 2(最大HPを増やす)です。", p);
            continue;
        }
        a = (int)strtol(key, &end, 10);
        if (end == key) { mod_log("[エラー] [HpScale] %s: 左側は TypeID(数字)か「最初-最後」で書いてください。", p); continue; }
        b = a;
        dash = end;
        while (*dash == ' ') dash++;
        if (*dash == '-') { b = (int)strtol(dash + 1, &end, 10); }
        v = strtod(eq + 1, &end);
        if (end == eq + 1 || v < 0.1 || v > 100.0) { mod_log("[エラー] [HpScale] %s: 倍率は 0.1〜100 で書いてください。", p); continue; }
        if (a < 0 || b > 255 || a > b) { mod_log("[エラー] [HpScale] %s: TypeID は 0〜255 です。", p); continue; }
        for (t = a; t <= b; t++) {
            if (mode == 2) hpScaleTable[t] = (v == 1.0) ? 0.0f : (float)v;
            else hpDmgTable[t] = (v == 1.0) ? 0.0f : (float)(1.0 / v);
        }
        if (mode == 2) { mod_log("[設定] HPの倍率(最大HPを増やす): TypeID %d〜%d を %.2f 倍", a, b, v); cfg.hpScaleCount++; }
        else { mod_log("[設定] HPの倍率(受けるダメージを減らす): TypeID %d〜%d の受けるダメージを 1/%.2f に", a, b, v); cfg.hpDmgCount++; }
    }
    if (cfg.hpScaleCount || cfg.hpDmgCount)
        mod_log("[設定] HPの倍率: %s", hpBodyOnly ? "武将本体だけ(随伴の兵士は元のまま)" : "随伴の兵士も含める");
}

static void load_config(void)
{
    snprintf(g_ini, sizeof(g_ini), "%s\\version.ini", g_dir);
    if (GetFileAttributesA(g_ini) == INVALID_FILE_ATTRIBUTES)
        mod_log("[情報] version.ini が見つからないので、全て既定の設定で動きます。");
    cfg.stealPatch = ini_int("StealPatch", 1);
    cfg.stealThreshold = ini_int("StealThreshold", 3);
    cfg.allyStrayZako = ini_int("AllyStrayZako", 1);
    cfg.allyEscort = ini_int("AllyEscort", 1);
    cfg.enemyEscort = ini_int("EnemyEscort", 1);
    cfg.enemyEscortRewrite = ini_int("EnemyEscortRewrite", 0);
    cfg.enemyLinkedZakoLevel = ini_int("EnemyLinkedZakoLevel", 4);
    cfg.baseEscortRewrite = ini_int("BaseEscortRewrite", 0);
    cfg.roamingEscortRewrite = ini_int("RoamingEscortRewrite", 0);
    cfg.nSquad = ini_list("SquadId", cfg.squad);
    cfg.nBaseSquad = ini_list("BaseSquadId", cfg.baseSquad);
    cfg.nRoamSquad = ini_list("RoamingSquadId", cfg.roamSquad);
    cfg.uniqueModelBoxes = ini_int("UniqueModelBoxes", 14);
    cfg.loadTableGuard = ini_int("LoadTableGuard", 1);
    cfg.loadTableGuardMinFree = ini_int("LoadTableGuardMinFree", 6);
    cfg.crashGuardDivZero = ini_int("CrashGuardDivZero", 1);
    cfg.crashGuardBadDraw = ini_int("CrashGuardBadDraw", 1);
    cfg.crashGuardNullFx = ini_int("CrashGuardNullFx", 1);
    cfg.crashGuardMotion = ini_int("CrashGuardMotion", 1);
    cfg.crashGuardBadId = ini_int("CrashGuardBadId", 1);
    cfg.crashGuardRedirect = ini_int("CrashGuardRedirect", 1);

    load_hp_scale();
    {
        char t[64];
        GetPrivateProfileStringA("Message", "BannerY", "348", t, sizeof(t), g_ini);
        cfg.bannerY = atof(t);
        if (cfg.bannerY < 0 || cfg.bannerY > 672) {
            mod_log("[エラー] [Message] BannerY は 0〜672 で指定してください(%s)。348(元の位置)にします。", t);
            cfg.bannerY = 348;
        }
        GetPrivateProfileStringA("Message", "TalkOffsetX", "0", t, sizeof(t), g_ini);
        cfg.talkOffsetX = atof(t);
        if (cfg.talkOffsetX < -600 || cfg.talkOffsetX > 900) {
            mod_log("[エラー] [Message] TalkOffsetX は -600〜900 で指定してください(%s)。0(元の位置)にします。", t);
            cfg.talkOffsetX = 0;
        }
        GetPrivateProfileStringA("Message", "TalkOffsetY", "0", t, sizeof(t), g_ini);
        cfg.talkOffsetY = atof(t);
        if (cfg.talkOffsetY < -300 || cfg.talkOffsetY > 300) {
            mod_log("[エラー] [Message] TalkOffsetY は -300〜300 で指定してください(%s)。0(元の位置)にします。", t);
            cfg.talkOffsetY = 0;
        }
    }
    cfg.mapToggleHidden = GetPrivateProfileIntA("Map", "ToggleHidden", 1, g_ini);
    {
        char t[64];
        GetPrivateProfileStringA("Affinity", "SlowSpeed", "0", t, sizeof(t), g_ini);
        cfg.slowSpeed = atof(t);
        if (cfg.slowSpeed < 0 || cfg.slowSpeed > 1) {
            mod_log("[エラー] [Affinity] SlowSpeed は 0〜1 で指定してください(%s)。0(元のまま)にします。", t);
            cfg.slowSpeed = 0;
        }
        if (cfg.slowSpeed > 0) mod_log("[設定] 相性のスロー: スロー中の速さを最低 %.0f%% にする", cfg.slowSpeed * 100);
        cfg.counterWindow = GetPrivateProfileIntA("Affinity", "CounterWindow", 1, g_ini);
        if (!cfg.counterWindow) mod_log("[設定] 相性負けの時の反撃の猶予(敵の攻撃の出始めのスロー): 無し");
    }
    {
        char t[64];
        GetPrivateProfileStringA("Map", "SurroundX", "1049", t, sizeof(t), g_ini);
        cfg.miniX = atof(t);
        if (cfg.miniX < 0 || cfg.miniX > 1280) {
            mod_log("[エラー] [Map] SurroundX は 0〜1280 で指定してください(%s)。1049(元の位置)にします。", t);
            cfg.miniX = 1049;
        }
        GetPrivateProfileStringA("Map", "SurroundY", "209", t, sizeof(t), g_ini);
        cfg.miniY = atof(t);
        if (cfg.miniY < 0 || cfg.miniY > 720) {
            mod_log("[エラー] [Map] SurroundY は 0〜720 で指定してください(%s)。209(元の位置)にします。", t);
            cfg.miniY = 209;
        }
        GetPrivateProfileStringA("Map", "SurroundSize", "128", t, sizeof(t), g_ini);
        cfg.miniSize = atof(t);
        if (cfg.miniSize < 32 || cfg.miniSize > 360) {
            mod_log("[エラー] [Map] SurroundSize は 32〜360 で指定してください(%s)。128(元の大きさ)にします。", t);
            cfg.miniSize = 128;
        }
    }
    if (cfg.enemyLinkedZakoLevel < 0 || cfg.enemyLinkedZakoLevel > 4) {
        mod_log("[エラー] EnemyLinkedZakoLevel は 0〜4 で指定してください(%d)。4(ゲーム本来)にします。", cfg.enemyLinkedZakoLevel);
        cfg.enemyLinkedZakoLevel = 4;
    }
    if (cfg.stealThreshold < 0 || cfg.stealThreshold > 150) {
        mod_log("[エラー] StealThreshold は 0〜150 で指定してください(%d)。3にします。", cfg.stealThreshold);
        cfg.stealThreshold = 3;
    }
    if (cfg.uniqueModelBoxes != 0 && (cfg.uniqueModelBoxes < 7 || cfg.uniqueModelBoxes > 16)) {
        mod_log("[エラー] UniqueModelBoxes は 0(使わない)か 7〜16 で指定してください(%d)。14にします。", cfg.uniqueModelBoxes);
        cfg.uniqueModelBoxes = 14;
    }
    if (cfg.loadTableGuardMinFree < 0 || cfg.loadTableGuardMinFree > 30) {
        mod_log("[エラー] LoadTableGuardMinFree は 0〜30 で指定してください(%d)。6にします。", cfg.loadTableGuardMinFree);
        cfg.loadTableGuardMinFree = 6;
    }
}

/* ================================================================
 *  書き換え
 * ================================================================ */
typedef struct {
    const char *name;
    DWORD off;
    int len;
    BYTE expect[16];
    void *hook;          /* NULL = 確かめるだけで書き換えない */
    DWORD *ret;          /* 割り込んだ場所の次の番地を入れる変数(無ければNULL) */
    void *dispVar;       /* NULL 以外: jmp ではなく、命令の中の番地(dispAt バイト目から4バイト)をこの変数の番地に書き換える */
    int dispAt;
    const BYTE *newBytes; /* NULL 以外: 命令ではなく固定の数値(.rdata)そのものを、この len バイトに書き換える */
} Site;

typedef struct {
    const char *name;
    int enabled;
    int done;            /* 1=入れた -1=入れられなかった */
    int vehOnly;         /* 1=命令は書き換えず、確かめた後で例外の受け止め役を登録するだけ */
    int dataOnly;        /* 1=固定の数値だけを書き換える(暗号化されていないので、Steamの保護が戻ったかの判断には使わない) */
    int nsite;
    Site site[8];
} Feature;

static Feature g_feat[32];
static int g_nfeat;

static Feature *add_feature(const char *name, int enabled)
{
    Feature *f = &g_feat[g_nfeat++];
    memset(f, 0, sizeof(*f));
    f->name = name;
    f->enabled = enabled;
    return f;
}

static void add_site(Feature *f, const char *name, DWORD off, int len, const BYTE *expect, void *hook, DWORD *ret)
{
    Site *s = &f->site[f->nsite++];
    s->name = name; s->off = off; s->len = len;
    memcpy(s->expect, expect, len);
    s->hook = hook; s->ret = ret;
}

/* 命令の中の「読む番地」だけを DLL の変数に向け替える場所(例: mov eax,[定数] → mov eax,[DLLの変数]) */
static void add_disp_site(Feature *f, const char *name, DWORD off, int len, const BYTE *expect, int dispAt, void *var)
{
    Site *s = &f->site[f->nsite++];
    s->name = name; s->off = off; s->len = len;
    memcpy(s->expect, expect, len);
    s->hook = NULL; s->ret = NULL;
    s->dispVar = var; s->dispAt = dispAt;
}

/* 固定の数値(.rdata、暗号化されていない)を書き換える場所 */
static void add_data_site(Feature *f, const char *name, DWORD off, int len, const BYTE *expect, const BYTE *newBytes)
{
    Site *s = &f->site[f->nsite++];
    s->name = name; s->off = off; s->len = len;
    memcpy(s->expect, expect, len);
    s->newBytes = newBytes;
    f->dataOnly = 1;
}

static int site_matches(const Site *s)
{
    return memcmp((const void *)(g_base + s->off), s->expect, s->len) == 0;
}
static int site_is_jmp(const Site *s)
{
    return *(const BYTE *)(g_base + s->off) == 0xE9;
}

/* 自分以外のスレッドを止める。止めたスレッドのハンドルを返す */
static int suspend_others(HANDLE *hs, int max)
{
    DWORD pid = GetCurrentProcessId(), tid = GetCurrentThreadId();
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te;
    int n = 0;
    if (snap == INVALID_HANDLE_VALUE) return 0;
    te.dwSize = sizeof(te);
    if (Thread32First(snap, &te)) {
        do {
            if (te.th32OwnerProcessID == pid && te.th32ThreadID != tid && n < max) {
                HANDLE h = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, FALSE, te.th32ThreadID);
                if (h) {
                    if (SuspendThread(h) != (DWORD)-1) hs[n++] = h;
                    else CloseHandle(h);
                }
            }
            te.dwSize = sizeof(te);
        } while (Thread32Next(snap, &te));
    }
    CloseHandle(snap);
    return n;
}
static void resume_all(HANDLE *hs, int n)
{
    int i;
    for (i = 0; i < n; i++) { ResumeThread(hs[i]); CloseHandle(hs[i]); }
}

/* 止めたスレッドのどれかが、書き換える範囲の途中(先頭以外)を実行中なら 1 */
static int any_thread_inside(HANDLE *hs, int n, const Feature *f)
{
    int i, k;
    for (i = 0; i < n; i++) {
        CONTEXT c;
        memset(&c, 0, sizeof(c));
        c.ContextFlags = CONTEXT_CONTROL;
        if (!GetThreadContext(hs[i], &c)) continue;
        for (k = 0; k < f->nsite; k++) {
            DWORD a = g_base + f->site[k].off;
            if (c.Eip > a && c.Eip < a + (DWORD)f->site[k].len) return 1;
        }
    }
    return 0;
}

/* ================================================================
 *  動きのデータの守り(FUN_009d08a0、Ghidra 0x9D08A0 = Launch.exe+2108A0)
 *  動きのデータ(EBX)や骨の一覧が読めない時に起きる不正アクセス(0x2108AC など)を例外として受け止め、
 *  その1回の骨の計算だけをやめて呼び出し元へ戻す。呼び出し元 FUN_00e18b80 が結果を
 *  先に「元の姿勢(大きさ1・回転なし・移動なし)」で埋めているので、その骨は1回だけ元の姿勢になる。
 *  普段(落ちない時)は何もしないので、速さに影響しない。
 * ================================================================ */
static DWORD cgMotionSkipCount, cgMotionLastEip, cgMotionLastAddr;
static LONG CALLBACK motion_veh(EXCEPTION_POINTERS *ep)
{
    DWORD eip, off;
    char *to;
    if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    eip = ep->ContextRecord->Eip;
    off = eip - g_base;
    if (off == 0x2108A9 || off == 0x2108AC || off == 0x2108AF)
        to = cg_motionExitA;           /* push ebp / FLD1 / push ebx の後 */
    else if (off == 0x2108C2)
        to = cg_motionExitB;           /* さらに push esi / push edi の後 */
    else
        return EXCEPTION_CONTINUE_SEARCH;
    cgMotionSkipCount++;
    cgMotionLastEip = off;
    cgMotionLastAddr = (DWORD)ep->ExceptionRecord->ExceptionInformation[1];
    ep->ContextRecord->Eip = (DWORD)to;
    return EXCEPTION_CONTINUE_EXECUTION;
}

/* ================================================================
 *  落ちた時の記録(mod_crash.txt)
 *  Launch.exe の中で不正アクセスなどが起きた瞬間(守りで受け止めなかったもの)の場所・レジスタ・
 *  スタックの先頭・各記録の値を書く。ゲームの動きは変えない(CONTINUE_SEARCH)。
 *  ※ゲーム自身が受け止める例外も書かれることがあるので、最大20件まで。
 * ================================================================ */
static volatile LONG g_crashLogged;
static DWORD g_imageSize, g_lastCrashEip, g_lastCrashEsp;
static LONG CALLBACK crash_veh(EXCEPTION_POINTERS *ep)
{
    char path[MAX_PATH], buf[2048];
    DWORD code = ep->ExceptionRecord->ExceptionCode, eip = ep->ContextRecord->Eip, w, i;
    CONTEXT *c = ep->ContextRecord;
    HANDLE h;
    int n;
    SYSTEMTIME t;
    if (code != EXCEPTION_ACCESS_VIOLATION && code != EXCEPTION_INT_DIVIDE_BY_ZERO && code != EXCEPTION_ILLEGAL_INSTRUCTION
        && code != EXCEPTION_STACK_OVERFLOW && code != EXCEPTION_PRIV_INSTRUCTION) return EXCEPTION_CONTINUE_SEARCH;
    if (eip < g_base || eip >= g_base + g_imageSize) return EXCEPTION_CONTINUE_SEARCH;
    if (eip == g_lastCrashEip && c->Esp == g_lastCrashEsp) return EXCEPTION_CONTINUE_SEARCH;   /* 同じ例外の2回目 */
    g_lastCrashEip = eip; g_lastCrashEsp = c->Esp;
    if (InterlockedIncrement(&g_crashLogged) > 20) return EXCEPTION_CONTINUE_SEARCH;
    GetLocalTime(&t);
    n = snprintf(buf, sizeof(buf),
        "==== %04d-%02d-%02d %02d:%02d:%02d  mod %s ====\r\n"
        "例外 0x%08lX  場所 Launch.exe+%06lX (Ghidra 0x%08lX)  読もうとした番地 0x%08lX\r\n"
        "EAX=%08lX EBX=%08lX ECX=%08lX EDX=%08lX ESI=%08lX EDI=%08lX EBP=%08lX ESP=%08lX\r\n"
        "スタック(Launch.exe内の番地は +XXXXXX で表示):\r\n",
        t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, MOD_VERSION,
        code, eip - g_base, eip - g_base + 0x7C0000,
        (DWORD)(ep->ExceptionRecord->NumberParameters >= 2 ? ep->ExceptionRecord->ExceptionInformation[1] : 0),
        c->Eax, c->Ebx, c->Ecx, c->Edx, c->Esi, c->Edi, c->Ebp, c->Esp);
    for (i = 0; i < 48 && n < (int)sizeof(buf) - 64; i++) {
        DWORD v;
        if (IsBadReadPtr((void *)(c->Esp + i * 4), 4)) break;
        v = *(DWORD *)(c->Esp + i * 4);
        if (v >= g_base && v < g_base + g_imageSize)
            n += snprintf(buf + n, sizeof(buf) - n, " [esp+%02lX] +%06lX(G %08lX)", i * 4, v - g_base, v - g_base + 0x7C0000);
        else
            n += snprintf(buf + n, sizeof(buf) - n, " [esp+%02lX] %08lX", i * 4, v);
        if (i % 4 == 3) n += snprintf(buf + n, sizeof(buf) - n, "\r\n");
    }
    n += snprintf(buf + n, sizeof(buf) - n,
        "\r\n記録: cgDrawSkipCount=%lu cgFxSkipCount=%lu cgIdSkipCount=%lu(最後の番号=%ld 呼び出し元=+%06lX) cgRecSkipCount=%lu cgRedirSkipCount=%lu cgRowSkipCount=%lu cgMotionSkipCount=%lu ltRejectCount=%lu ltFullCount=%lu\r\n\r\n",
        cgDrawSkipCount, cgFxSkipCount, cgIdSkipCount, (long)cgIdLastId, cgIdLastCaller, cgRecSkipCount, cgRedirSkipCount, cgRowSkipCount, cgMotionSkipCount, ltRejectCount, ltFullCount);
    snprintf(path, sizeof(path), "%s\\mod_crash.txt", g_dir);
    h = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) { WriteFile(h, buf, (DWORD)n, &w, NULL); CloseHandle(h); }
    return EXCEPTION_CONTINUE_SEARCH;
}

static int install_feature(Feature *f)
{
    static HANDLE hs[1024];
    int n, k, tries;
    if (f->vehOnly) return AddVectoredExceptionHandler(1, motion_veh) != NULL;
    for (k = 0; k < f->nsite; k++)
        if (f->site[k].ret) *f->site[k].ret = g_base + f->site[k].off + f->site[k].len;
    for (tries = 0; tries < 200; tries++) {
        n = suspend_others(hs, 1024);
        if (!any_thread_inside(hs, n, f)) break;
        resume_all(hs, n);
        n = -1;
        Sleep(5);
    }
    if (n < 0) return 0;
    for (k = 0; k < f->nsite; k++) {
        Site *s = &f->site[k];
        BYTE *p = (BYTE *)(g_base + s->off);
        DWORD old, rel = (DWORD)s->hook - (DWORD)(p + 5);
        int i;
        if (s->newBytes) {
            VirtualProtect(p, s->len, PAGE_EXECUTE_READWRITE, &old);   /* 命令の場所にも使うので実行も許す */
            memcpy(p, s->newBytes, s->len);
            VirtualProtect(p, s->len, old, &old);
            FlushInstructionCache(GetCurrentProcess(), p, s->len);
            continue;
        }
        if (s->dispVar) {
            DWORD v = (DWORD)s->dispVar;
            VirtualProtect(p, s->len, PAGE_EXECUTE_READWRITE, &old);
            memcpy(p + s->dispAt, &v, 4);
            VirtualProtect(p, s->len, old, &old);
            FlushInstructionCache(GetCurrentProcess(), p, s->len);
            continue;
        }
        if (!s->hook) continue;
        VirtualProtect(p, s->len, PAGE_EXECUTE_READWRITE, &old);
        p[0] = 0xE9;
        memcpy(p + 1, &rel, 4);
        for (i = 5; i < s->len; i++) p[i] = 0x90;
        VirtualProtect(p, s->len, old, &old);
        FlushInstructionCache(GetCurrentProcess(), p, s->len);
    }
    resume_all(hs, n);
    return 1;
}

static void setup_features(void)
{
    static const BYTE b_steal[] = {0xE8, 0xB9, 0x96, 0xFA, 0xFF, 0x84, 0xC0, 0x75, 0xE4};
    static const BYTE b_mp1[] = {0x75, 0x0D, 0x38, 0x5E, 0x10, 0x74, 0x08, 0x89, 0x46, 0x0C, 0x88, 0x5E, 0x10, 0xEB, 0x16};
    static const BYTE b_mp2[] = {0x8B, 0x46, 0x08, 0x40, 0x25, 0x01, 0x00, 0x00, 0x80};
    static const BYTE b_mp3[] = {0x79, 0x05, 0x48, 0x83, 0xC8, 0xFE, 0x40, 0x89, 0x46, 0x0C, 0x89, 0x46, 0x08};
    static BYTE b_mp4[] = {0x8B, 0x15, 0, 0, 0, 0};
    static const BYTE b_sl1[] = {0xFF, 0xD0};
    static const BYTE b_sl2[] = {0xD8, 0x4C, 0x24, 0x10, 0xD9, 0x5C, 0x24, 0x10};
    static const BYTE b_sl3[] = {0x80, 0xBF, 0x08, 0x03, 0x00, 0x00, 0x00};
    static const BYTE b_ecReg[] = {0x51, 0x3D, 0x58, 0x02, 0x00, 0x00};
    static const BYTE b_ecBS[] = {0x8B, 0x44, 0x24, 0x30, 0x83, 0x7C, 0xF0, 0x04, 0x00};
    static const BYTE b_ecBM[] = {0x8B, 0xC2, 0xD1, 0xE8, 0x25, 0x55, 0x55, 0x55, 0x55};
    static const BYTE b_prolog[] = {0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8};
    static const BYTE b_ltFull[] = {0x83, 0xFD, 0x5A, 0x72, 0xF1};
    static const BYTE b_cgDiv[] = {0xDA, 0x74, 0x24, 0x10, 0xF7, 0xF6};
    static const BYTE b_cgDraw[] = {0x8B, 0x03, 0x8B, 0x50, 0x08};
    static const BYTE b_cgPart[] = {0x8B, 0x4C, 0x24, 0x0C, 0x89, 0x44, 0x24, 0x10};
    /* 0x9D08A0〜0x9D08C7: push ebp / mov ebp,esp / fld1 / push ebx / mov ebx,[ebp+8] / mov eax,[ebx+20] /
       mov eax,[eax+ecx*4] / mov edx,[ebx+24] / push esi / mov esi,eax / shr eax,10 / push edi /
       lea edi,[edx+eax*4] / mov eax,[ebp+C] / and esi,F / cmp word ptr [edi],0 / jnz */
    static const BYTE b_motion1[] = {0x55, 0x8B, 0xEC, 0xD9, 0xE8, 0x53, 0x8B, 0x5D, 0x08, 0x8B, 0x43, 0x20, 0x8B, 0x04, 0x88};
    static const BYTE b_motion2[] = {0x8B, 0x53, 0x24, 0x56, 0x8B, 0xF0, 0xC1, 0xE8, 0x10, 0x57, 0x8D, 0x3C, 0x82, 0x8B, 0x45};
    static const BYTE b_motion3[] = {0x0C, 0x83, 0xE6, 0x0F, 0x66, 0x83, 0x3F, 0x00, 0x75, 0x3B};
    static const BYTE b_badId1[] = {0x8D, 0x88, 0xD4, 0xFE, 0xFF, 0xFF, 0x81, 0xF9, 0x95, 0x00, 0x00, 0x00};
    static const BYTE b_badId2[] = {0x3D, 0x96, 0x00, 0x00, 0x00, 0x72, 0x06};
    static const BYTE b_badId3[] = {0x33, 0xC0, 0x8B, 0x40, 0x28, 0xC3};
    static const BYTE b_rec1[] = {0x8B, 0x73, 0x08, 0x8B, 0x46, 0x10, 0xE8, 0x7F, 0xC2, 0xF8, 0xFF};
    static const BYTE b_rec2[] = {0x3D, 0x37, 0x09, 0x00, 0x00};
    static const BYTE b_rec3[] = {0x77, 0x0D, 0x8D, 0x14, 0xC0, 0xA1};
    static const BYTE b_rec5[] = {0x8D, 0x04, 0xD0, 0xEB, 0x02, 0x33, 0xC0, 0x0F, 0xBF, 0x40, 0x0E};
    BYTE b_rec4[4];
    static const BYTE b_rd1[] = {0xE8, 0x86, 0xAA, 0xFF, 0xFF};                                 /* call FUN_00dab7e0 */
    static const BYTE b_rdEdi[] = {0x8B, 0xBC, 0x86, 0xDC, 0x00, 0x00, 0x00};                   /* mov edi,[esi+eax*4+DC] */
    static const BYTE b_rd2[] = {0xE8, 0xE9, 0xA9, 0xFF, 0xFF, 0x8B, 0x54, 0x24, 0x10};         /* call / mov edx,[esp+10] */
    static const BYTE b_rdEsi[] = {0x8B, 0xB4, 0x82, 0xDC, 0x00, 0x00, 0x00};                   /* mov esi,[edx+eax*4+DC] */
    static const BYTE b_rd3[] = {0xE8, 0x96, 0xA9, 0xFF, 0xFF, 0x8B, 0x54, 0x24, 0x10};
    static const BYTE b_rdEbx[] = {0x8B, 0x9C, 0x82, 0xDC, 0x00, 0x00, 0x00};                   /* mov ebx,[edx+eax*4+DC] */
    static const BYTE b_row2[] = {0x8B, 0x4B, 0x0C, 0x8B, 0x53, 0x08, 0x55};                    /* mov ecx,[ebx+C] / mov edx,[ebx+8] / push ebp */
    static const BYTE b_row3[] = {0x8B, 0xAC, 0x88, 0xDC, 0x00, 0x00, 0x00};                    /* mov ebp,[eax+ecx*4+DC] */
    static const BYTE b_row4[] = {0x89, 0x74, 0x24, 0x10, 0x85, 0xED, 0x0F, 0x84};              /* mov [esp+10],esi / test ebp,ebp / jz */
    BYTE b_row1[5] = {0xA1};
    static const BYTE b_hp1[] = {0xE8, 0x78, 0x69, 0xFF, 0xFF};        /* call FUN_00f27a20 */
    static const BYTE b_hp2[] = {0x80, 0x7C, 0x24, 0x1F, 0x00};        /* cmp byte ptr [esp+1F],0(本体か) */
    static const BYTE b_hp3[] = {0xD9, 0x44, 0x24, 0x48};              /* fld dword ptr [esp+48](最大HP) */
    static const BYTE b_dmg1[] = {0x83, 0xEC, 0x08, 0x3D, 0x96, 0x00, 0x00, 0x00};              /* sub esp,8 / cmp eax,96 */
    static const BYTE b_dmg3[] = {0x8D, 0x0C, 0xC5, 0x00, 0x00, 0x00, 0x00, 0x2B, 0xC8, 0x8D, 0x8C, 0xCA, 0x80, 0x5B, 0x07, 0x00};
    BYTE b_dmg2[6] = {0x8B, 0x15};                                                             /* mov edx,[DAT_011839e0] */
    BYTE b_bn1[5] = {0xA1}, b_bn2[6] = {0xD9, 0x05}, b_bn3[6] = {0xD9, 0x05}, b_bn4[6] = {0xD9, 0x05}, b_bn5[6] = {0xD9, 0x05};
    static const BYTE b_bn6[] = {0x8B, 0x0D};
    BYTE b_tk1[6] = {0xDC, 0x05}, b_tk2[6] = {0xDC, 0x05}, b_tk4[6] = {0xD9, 0x05};
    BYTE b_fr1[6] = {0xDC, 0x05}, b_fr2[6] = {0xD9, 0x05}, b_fr3[6] = {0xD9, 0x05};
    static const BYTE b_tk3[] = {0x25, 0x00, 0x00, 0x40, 0x00};
    BYTE b_ecMem[7] = {0x8B, 0x04, 0x85};
    DWORD disp = g_base + 0x9C5DB0;
    Feature *f;
    int escortNeeded, rw, brw, rrw;
    static const DWORD pct[5] = {0, 30, 50, 70, 100};

    memcpy(b_ecMem + 3, &disp, 4);
    disp = g_base + 0x84F97C; memcpy(b_bn1 + 1, &disp, 4);   /* DAT_0100f97c */
    disp = g_base + 0x8A1F48; memcpy(b_tk1 + 2, &disp, 4);   /* DAT_01061f48 */
    disp = g_base + 0x8534C8; memcpy(b_tk4 + 2, &disp, 4);   /* DAT_010134c8 */
    disp = g_base + 0x8A15B8; memcpy(b_fr1 + 2, &disp, 4);   /* DAT_010615b8 */
    disp = g_base + 0x85AD20; memcpy(b_fr2 + 2, &disp, 4);   /* DAT_0101ad20 */
    disp = g_base + 0x8A1BB0; memcpy(b_fr3 + 2, &disp, 4);   /* DAT_01061bb0 */
    disp = g_base + 0x8A1F40; memcpy(b_tk2 + 2, &disp, 4);   /* DAT_01061f40 */
    disp = g_base + 0x8A079C; memcpy(b_bn2 + 2, &disp, 4);   /* DAT_0106079c */
    disp = g_base + 0x8A0280; memcpy(b_bn3 + 2, &disp, 4);   /* DAT_01060280 */
    disp = g_base + 0x8A0774; memcpy(b_bn4 + 2, &disp, 4);   /* DAT_01060774 */
    disp = g_base + 0x8A0280; memcpy(b_bn5 + 2, &disp, 4);
    disp = g_base + 0x9C39E0;
    memcpy(b_dmg2 + 2, &disp, 4);
    memcpy(b_mp4 + 2, &disp, 4);              /* mov edx,[DAT_011839e0] */
    disp = g_base + 0x9C3A28;                 /* mov eax,[DAT_01183a28](描画の表) */
    memcpy(b_row1 + 1, &disp, 4);
    disp = g_base + 0x9C3BDC;                 /* mov eax,[DAT_01183bdc](武将の記録の表) */
    memcpy(b_rec4, &disp, 4);

    /* 1. 横取りパッチ */
    stThreshold = (DWORD)cfg.stealThreshold;
    f = add_feature("横取りパッチ v4", cfg.stealPatch != 0);
    add_site(f, "+77B6B2", 0x77B6B2, 9, b_steal, hk_steal, &R_steal);

    /* 2. 随伴の制御 */
    rw = cfg.enemyEscortRewrite && cfg.nSquad > 0;
    brw = cfg.baseEscortRewrite && cfg.nBaseSquad > 0;
    rrw = cfg.roamingEscortRewrite && cfg.nRoamSquad > 0;
    if (cfg.enemyEscortRewrite && cfg.nSquad <= 0) mod_log("[情報] SquadId が空か不正なので、敵随伴の書き換えはしません。");
    if (cfg.baseEscortRewrite && cfg.nBaseSquad <= 0) mod_log("[情報] BaseSquadId が空か不正なので、拠点兵長の随伴の書き換えはしません。");
    if (cfg.roamingEscortRewrite && cfg.nRoamSquad <= 0) mod_log("[情報] RoamingSquadId が空か不正なので、遊撃隊長の随伴の書き換えはしません。");
    ecMasterEnabled = 1;
    ecAllyEscortEnabled = cfg.allyEscort ? 1 : 0;
    ecEnemyEscortEnabled = cfg.enemyEscort ? 1 : 0;
    ecRewriteEnabled = rw ? 1 : 0;
    ecRewriteListCount = rw ? (DWORD)cfg.nSquad : 0;
    if (rw) memcpy(ecRewriteList, cfg.squad, sizeof(DWORD) * cfg.nSquad);
    ecBaseRewriteEnabled = brw ? 1 : 0;
    ecBaseRewriteListCount = brw ? (DWORD)cfg.nBaseSquad : 0;
    if (brw) memcpy(ecBaseRewriteList, cfg.baseSquad, sizeof(DWORD) * cfg.nBaseSquad);
    ecRoamRewriteEnabled = rrw ? 1 : 0;
    ecRoamRewriteListCount = rrw ? (DWORD)cfg.nRoamSquad : 0;
    if (rrw) memcpy(ecRoamRewriteList, cfg.roamSquad, sizeof(DWORD) * cfg.nRoamSquad);
    ecEnemyLinkedPercent = pct[cfg.enemyLinkedZakoLevel];
    escortNeeded = !cfg.allyEscort || !cfg.enemyEscort || rw || brw || rrw || cfg.enemyLinkedZakoLevel < 4;
    f = add_feature("随伴の制御 v5", escortNeeded);
    add_site(f, "+7891A0", 0x7891A0, 6, b_ecReg, hk_ecReg, &R_ecReg);
    add_site(f, "+770F34", 0x770F34, 7, b_ecMem, hk_ecMember, &R_ecMember);
    add_site(f, "+6DA010", 0x6DA010, 9, b_ecBS, hk_ecBudgetSkip, &R_ecBudgetSkip);
    add_site(f, "+6DA1A4", 0x6DA1A4, 9, b_ecBM, hk_ecBudgetMember, &R_ecBudgetMember);

    /* 3. 固有武将の表示上限 */
    pbToA = (DWORD)cfg.uniqueModelBoxes; pbToB = 0; pbToN = 0;
    f = add_feature("固有武将の表示上限", cfg.uniqueModelBoxes != 0);
    add_site(f, "+659AF0", 0x659AF0, 6, b_prolog, hk_umb, &R_umb);

    /* 4. 読み込みの表の守り */
    ltMinFree = (DWORD)cfg.loadTableGuardMinFree;
    f = add_feature("読み込みの表の守り", cfg.loadTableGuard != 0);
    add_site(f, "+657480", 0x657480, 6, b_prolog, hk_ltWait, &R_ltWait);
    add_site(f, "+5D9ADB", 0x5D9ADB, 5, b_ltFull, hk_ltFull, NULL);

    /* 5. クラッシュの守り(3つは別々に入れ切りできる) */
    f = add_feature("クラッシュの守り(0での割り算)", cfg.crashGuardDivZero != 0);
    add_site(f, "+7FB6B8", 0x7FB6B8, 6, b_cgDiv, hk_cgDiv, &R_cgDiv);
    f = add_feature("クラッシュの守り(描画の物体)", cfg.crashGuardBadDraw != 0);
    add_site(f, "+5F0E59", 0x5F0E59, 5, b_cgDraw, hk_cgDraw, &R_cgDraw);
    add_site(f, "+5F0E6F", 0x5F0E6F, 8, b_cgPart, hk_cgPart, &R_cgPart);
    f = add_feature("クラッシュの守り(演出の部品)", cfg.crashGuardNullFx != 0);
    add_site(f, "+625030", 0x625030, 6, b_prolog, hk_cgFx, &R_cgFx);

    /* 6. 動きのデータの守り: FUN_009d08a0 の先頭〜0x9D08C6 が想定どおりの時だけ受け止め役を登録する */
    f = add_feature("クラッシュの守り(動きのデータ)", cfg.crashGuardMotion != 0);
    f->vehOnly = 1;
    add_site(f, "+2108A0", 0x2108A0, 15, b_motion1, NULL, NULL);
    add_site(f, "+2108AF", 0x2108AF, 15, b_motion2, NULL, NULL);
    add_site(f, "+2108BE", 0x2108BE, 10, b_motion3, NULL, NULL);

    /* 9. HPの倍率: FUN_00f30e90(Ghidra 0xF30E90、戦場への登録)で FUN_00f27a20 が最大HP(小数、[esp+48])を
          計算した直後(0xF310A8)に、600番の行の TypeID(+0x44)が対象なら倍にする。今のHPは「最大HP×割合」で
          この後に計算されるので同じ倍率になる。登録のたびに計算し直されるので重ねがけにならない。 */
    f = add_feature("HPの倍率(最大HPを増やす)", cfg.hpScaleCount > 0);
    add_site(f, "+7710A3", 0x7710A3, 5, b_hp1, NULL, NULL);
    add_site(f, "+7710A8", 0x7710A8, 5, b_hp2, hk_hpScale, &R_hpScale);
    add_site(f, "+77113A", 0x77113A, 4, b_hp3, NULL, NULL);

    /* 11. 真ん中の帯の縦の位置: FUN_00eeeba0(Ghidra 0xEEEBA0、8種類の帯の全てが通る)の中で、帯の背景の四角の上端(348、
           特別なモードでは336)と、一緒に出す演出の縦の位置(360 / 348)を読む5つの命令の「読む番地」を、DLL の変数に
           向け替える。定数そのもの(他の画面でも使われる)は変えない。画面は横1280×縦720の座標。 */
    {
        float d = (float)(cfg.bannerY - 348.0);
        msgBandY = 348.0f + d; msgBandY2 = 336.0f + d; msgFxY = 360.0f + d; msgFxY2 = 348.0f + d;
    }
    f = add_feature("真ん中の帯の位置", cfg.bannerY != 348.0);
    add_disp_site(f, "+72ECAA", 0x72ECAA, 5, b_bn1, 1, &msgBandY);    /* mov eax,[DAT_0100f97c] (348) */
    add_disp_site(f, "+72EC98", 0x72EC98, 6, b_bn2, 2, &msgBandY2);   /* fld [DAT_0106079c] (336) */
    add_disp_site(f, "+72ED41", 0x72ED41, 6, b_bn3, 2, &msgFxY);      /* fld [DAT_01060280] (360) */
    add_disp_site(f, "+72ED53", 0x72ED53, 6, b_bn4, 2, &msgFxY2);     /* fld [DAT_01060774] (348) */
    add_disp_site(f, "+72ED73", 0x72ED73, 6, b_bn5, 2, &msgFxY);      /* fld [DAT_01060280] (360) */
    add_site(f, "+72ECB7", 0x72ECB7, 2, b_bn6, NULL, NULL);           /* mov ecx,[DAT_0100f980] の先頭 */

    /* 12. 会話の窓の縦の位置: FUN_00eee650(Ghidra 0xEEE650、会話の窓を作る)はセリフの縦の位置を
           78 − (文の高さ×0.5) + 296 または 371(double、[DAT_01061f48] / [DAT_01061f40])で決める。この2つの
           「読む番地」を DLL の変数(+TalkOffsetY)に向け替える。横は 223。 */
    msgTalkY1 = 296.0 + cfg.talkOffsetY;
    msgTalkY2 = 371.0 + cfg.talkOffsetY;
    msgTalkX = (float)(223.0 + cfg.talkOffsetX);
    /* 枠・顔の絵・名前の欄は、毎フレーム描く FUN_00ed76f0(Ghidra 0xED76F0)が 横 68([DAT_010615b8]、double)と
       縦 371 / 296([DAT_01061bb0] / [DAT_0101ad20]、float)を基準に置く。ここも同じ量だけずらす。 */
    msgFrameX = 68.0 + cfg.talkOffsetX;
    msgFrameY1 = (float)(296.0 + cfg.talkOffsetY);
    msgFrameY2 = (float)(371.0 + cfg.talkOffsetY);
    f = add_feature("会話の窓の位置", cfg.talkOffsetY != 0.0 || cfg.talkOffsetX != 0.0);
    add_disp_site(f, "+72EA56", 0x72EA56, 6, b_tk4, 2, &msgTalkX);    /* fld [DAT_010134c8] (横 223) */
    add_disp_site(f, "+7177AB", 0x7177AB, 6, b_fr1, 2, &msgFrameX);   /* fadd double ptr [DAT_010615b8] (枠の横 68) */
    add_disp_site(f, "+7177D0", 0x7177D0, 6, b_fr2, 2, &msgFrameY1);  /* fld [DAT_0101ad20] (枠の縦 296) */
    add_disp_site(f, "+7177D8", 0x7177D8, 6, b_fr3, 2, &msgFrameY2);  /* fld [DAT_01061bb0] (枠の縦 371) */
    add_disp_site(f, "+72EAC3", 0x72EAC3, 6, b_tk1, 2, &msgTalkY1);   /* fadd double ptr [DAT_01061f48] (296) */
    add_disp_site(f, "+72EACF", 0x72EACF, 6, b_tk2, 2, &msgTalkY2);   /* fadd double ptr [DAT_01061f40] (371) */
    add_site(f, "+72EAB4", 0x72EAB4, 5, b_tk3, NULL, NULL);            /* and eax,400000 */

    /* 13. 地図の切り替え: FUN_00f13400(Ghidra 0xF13400、毎フレームの地図の処理)は、切り替えのボタン
           (キーボード[.] / Xbox の View など)が押されると、地図の物体の +0x8(前の状態)から
           「(前の状態+1)を2で割った余り」を +0xC(今の状態: 0=全体 1=周辺)に入れる。+0xC が 2 だと地図は描かれない
           (毒弓兵の秘計の時と同じ)。ここを 0 → 1 → 2 → 0 の順に回るようにする。 */
    mapModeCount = 3;
    R_mapToggle = g_base + 0x7534B0;
    f = add_feature("地図の切り替えに非表示を足す", cfg.mapToggleHidden != 0);
    add_site(f, "+75348B", 0x75348B, 15, b_mp1, NULL, NULL);         /* jnz / cmp [esi+10],bl / jz / mov [esi+0C],eax ... */
    add_site(f, "+75349A", 0x75349A, 9, b_mp2, hk_mapToggle, NULL);  /* mov eax,[esi+8] / inc eax / and eax,80000001 */
    add_site(f, "+7534A3", 0x7534A3, 13, b_mp3, NULL, NULL);         /* jns / dec / or / inc / mov [esi+0C],eax / mov [esi+8],eax */
    add_site(f, "+7534B0", 0x7534B0, 6, b_mp4, NULL, NULL);          /* mov edx,[DAT_011839e0](戻り先) */

    /* 14. 周辺地図の位置と大きさ: FUN_00ec1270(Ghidra 0xEC1270、毎フレーム)は周辺地図の四角を
           中心の横 [DAT_01060560]=1049(float)、中心の縦 [DAT_01061510]=209(double)、中心から端まで [DAT_01014354]=128(float)
           で決める(1人プレイの時。2人プレイは 1055.5 / 197.5 を使い、ここでは変えない)。CE で直接この3つの数値を
           変えて実機確認済み(2026-11-03)なので、同じく数値そのものを書き換える。 */
    {
        static const BYTE e_x[4] = {0x00, 0x20, 0x83, 0x44};                          /* 1049.0f */
        static const BYTE e_y[8] = {0, 0, 0, 0, 0, 0x20, 0x6A, 0x40};                 /* 209.0 */
        static const BYTE e_s[4] = {0x00, 0x00, 0x00, 0x43};                          /* 128.0f */
        static BYTE n_x[4], n_y[8], n_s[4];
        float fx = (float)cfg.miniX, fs = (float)cfg.miniSize;
        double dy = cfg.miniY;
        memcpy(n_x, &fx, 4); memcpy(n_y, &dy, 8); memcpy(n_s, &fs, 4);
        f = add_feature("周辺地図の位置と大きさ", cfg.miniX != 1049.0 || cfg.miniY != 209.0 || cfg.miniSize != 128.0);
        add_data_site(f, "+8A0560", 0x8A0560, 4, e_x, n_x);
        add_data_site(f, "+8A1510", 0x8A1510, 8, e_y, n_y);
        add_data_site(f, "+854354", 0x854354, 4, e_s, n_s);
    }

    /* 15. 相性のスローの速さ: FUN_00e14400(Ghidra 0xE14400)は武将ごとの「1回の更新で進む時間」(行動の部品 +0x20、普段 1.0)を、
           いくつかの倍率を掛けて決める。その中の「状態 0x0C の効果」(状態の一覧の種類 0x0C、+0x2F0 が立っている時)が
           相性で負けている時の敵の攻撃・武器切り替え攻撃・反撃の演出のスロー(CE の試験で確認、2026-11-03)。
           その倍率が SlowSpeed より小さい時は SlowSpeed にする(1 = スロー無し、0 = 元のまま)。 */
    slowMin = (float)cfg.slowSpeed;
    f = add_feature("相性のスローの速さ", cfg.slowSpeed > 0);
    add_site(f, "+6545F5", 0x6545F5, 2, b_sl1, NULL, NULL);            /* call eax(効果->+0xC) */
    add_site(f, "+6545F7", 0x6545F7, 8, b_sl2, hk_slowCap, &R_slowCap); /* fmul [esp+10] / fstp [esp+10] */
    add_site(f, "+6545FF", 0x6545FF, 7, b_sl3, NULL, NULL);            /* cmp byte ptr [edi+308],0 */

    /* 16. 反撃の猶予の有無: FUN_00e2e300(Ghidra 0xE2E300、呼び出し元 FUN_00e36860)は、その武将の相性がプレイヤーより有利で
           近くにいれば 1 を返し、敵のチャージ攻撃・武器切り替え攻撃の出始めに「武器切り替えで反撃できる猶予(スロー)」を作る。
           CounterWindow=0 の時は、比べた結果の分岐 jnz を必ず飛ぶ jmp にして、常に 0 を返させる(CE の試験で確認、2026-11-04)。 */
    {
        static const BYTE e_cw0[] = {0x83, 0xF8, 0x02};
        static const BYTE e_cw1[] = {0x0F, 0x85, 0x90, 0x00, 0x00, 0x00};
        static const BYTE n_cw1[] = {0xE9, 0x91, 0x00, 0x00, 0x00, 0x90};
        f = add_feature("反撃の猶予を無くす", cfg.counterWindow == 0);
        add_data_site(f, "+66E34B", 0x66E34B, 6, e_cw1, n_cw1);
        add_site(f, "+66E348", 0x66E348, 3, e_cw0, NULL, NULL);    /* cmp eax,2 */
    }

    /* 10. HPの倍率(受けるダメージを減らす): FUN_00ea78e0(Ghidra 0xEA78E0、HPを増減させる関数。EAX=参加者の番号、
           引数=増減量の小数)の入口で、増減量が負(ダメージ)で、その武将の TypeID が対象なら、増減量に 1/倍率 を掛ける。
           ゲージの長さ(最大HP)は元のまま。参照元12か所の全てのダメージ・回復がここを通る。 */
    f = add_feature("HPの倍率(受けるダメージを減らす)", cfg.hpDmgCount > 0);
    add_site(f, "+6E78E0", 0x6E78E0, 8, b_dmg1, hk_hpDmg, &R_hpDmg);
    add_site(f, "+6E78EA", 0x6E78EA, 6, b_dmg2, NULL, NULL);
    add_site(f, "+6E78F0", 0x6E78F0, 16, b_dmg3, NULL, NULL);

    /* 8. 付け替え先なしの守り: 描画の FUN_00db0d10 / FUN_00db0d80 は、物体を別の行へ付け替える
          FUN_00dab7e0 が「見つからない」(-1)を返しても確かめず、表の -1 番目(=0x2378 の入った欄)を
          物体として使って落ちる(0x5F0D61 / 0x5F0E59 / 0x5F0E66 のクラッシュ)。-1 の時は付け替えをやめ、元の物体のまま続ける。 */
    f = add_feature("クラッシュの守り(付け替え先なし)", cfg.crashGuardRedirect != 0);
    add_site(f, "+5F0D55", 0x5F0D55, 5, b_rd1, NULL, NULL);
    add_site(f, "+5F0D5A", 0x5F0D5A, 7, b_rdEdi, hk_cgRedir1, &R_cgRedir1);
    add_site(f, "+5F0DF2", 0x5F0DF2, 9, b_rd2, NULL, NULL);
    add_site(f, "+5F0DFB", 0x5F0DFB, 7, b_rdEsi, hk_cgRedir2, &R_cgRedir2);
    add_site(f, "+5F0E45", 0x5F0E45, 9, b_rd3, NULL, NULL);
    add_site(f, "+5F0E4E", 0x5F0E4E, 7, b_rdEbx, hk_cgRedir3, &R_cgRedir3);
    /* FUN_00e26d70(0xE26D70): 記録の +0xC の行が -1(居ない馬)でも表を読み、0x2378 を物体として使う
       (0x666E22 のクラッシュ)。行が負なら物体を 0 にして、ゲーム自身の「物体が無い」時の流れ(0xE2707E)へ進ませる。 */
    f = add_feature("クラッシュの守り(馬の居ない記録)", cfg.crashGuardRedirect != 0);
    add_site(f, "+666D89", 0x666D89, 5, b_row1, NULL, NULL);
    add_site(f, "+666D8E", 0x666D8E, 7, b_row2, NULL, NULL);
    add_site(f, "+666D95", 0x666D95, 7, b_row3, hk_cgRow, &R_cgRow);
    add_site(f, "+666DA4", 0x666DA4, 8, b_row4, NULL, NULL);

    /* 7. 範囲外の番号の守り: FUN_00dd6d30(Ghidra 0xDD6D30)は番号が 150〜299 か 450 以上(1802以外)だと
          わざと 0 番地を読んで落ちる(0xDD6D74 `xor eax,eax / mov eax,[eax+28]`)。そこで -1(「無し」、
          同じ関数が 300〜449 で中身が無い時に返す値)を返す。 */
    f = add_feature("クラッシュの守り(範囲外の番号)", cfg.crashGuardBadId != 0);
    add_site(f, "+616D30", 0x616D30, 12, b_badId1, NULL, NULL);
    add_site(f, "+616D6D", 0x616D6D, 7, b_badId2, NULL, NULL);
    add_site(f, "+616D74", 0x616D74, 6, b_badId3, hk_cgBadId, NULL);
    /* 呼び出し元 FUN_00e4a080(0xE4AAAC で呼ぶ)は、結果が 0x937 を超えると 0 番地+0xE を読んで落ちる
       (0x68AAC7 のクラッシュ。ゲームが -1 を返す時も同じ)。そこで、範囲外の時は中身が全て0の
       「空の武将の記録」を読ませる(+0xE が 0 = 特別な武将ではない、の扱いになる)。 */
    add_site(f, "+68AAA6", 0x68AAA6, 11, b_rec1, NULL, NULL);
    add_site(f, "+68AAB1", 0x68AAB1, 5, b_rec2, hk_cgCharRec, &R_cgCharRec);
    add_site(f, "+68AAB6", 0x68AAB6, 6, b_rec3, NULL, NULL);
    add_site(f, "+68AABC", 0x68AABC, 4, b_rec4, NULL, NULL);
    add_site(f, "+68AAC0", 0x68AAC0, 11, b_rec5, NULL, NULL);
}

/* ================================================================
 *  味方の野良雑魚兵(AllyStrayZako=0 の時、2つの値を0にし続ける)
 * ================================================================ */
static DWORD g_strayResetCount;
static void keep_stray_zero(void)
{
    static const DWORD offs[2] = {0x8A0D80, 0x84CA60};
    int i;
    for (i = 0; i < 2; i++) {
        volatile float *p = (volatile float *)(g_base + offs[i]);
        if (*p != 0.0f) {
            DWORD old;
            if (VirtualProtect((void *)p, 4, PAGE_READWRITE, &old)) {
                *p = 0.0f;
                VirtualProtect((void *)p, 4, old, &old);
                g_strayResetCount++;
            }
        }
    }
}

static void write_status(void)
{
    char path[MAX_PATH];
    FILE *f;
    SYSTEMTIME t;
    snprintf(path, sizeof(path), "%s\\mod_status.txt", g_dir);
    f = fopen(path, "wb");
    if (!f) return;
    GetLocalTime(&t);
    fprintf(f, "mod %s  最終更新 %04d-%02d-%02d %02d:%02d:%02d(30秒ごとに更新)\r\n\r\n",
            MOD_VERSION, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
    fprintf(f, "[横取りパッチ] hookHitCount=%lu fullCount=%lu allyFullCount=%lu stealSuccessCount=%lu stealFailCount=%lu\r\n",
            hookHitCount, fullCount, allyFullCount, stealSuccessCount, stealFailCount);
    fprintf(f, "[随伴の制御] ecHookFireCount=%lu ecBlockedCount=%lu ecLinkedBlockedCount=%lu ecRewrittenCount=%lu ecMemberRewrittenCount=%lu\r\n"
               "             ecBudgetSkipCount=%lu ecBudgetMemberClearedCount=%lu ecBaseRewrittenCount=%lu ecRoamRewrittenCount=%lu\r\n",
            ecHookFireCount, ecBlockedCount, ecLinkedBlockedCount, ecRewrittenCount, ecMemberRewrittenCount,
            ecBudgetSkipCount, ecBudgetMemberClearedCount, ecBaseRewrittenCount, ecRoamRewrittenCount);
    fprintf(f, "[味方の野良雑魚兵] 0に戻した回数=%lu\r\n", g_strayResetCount);
    fprintf(f, "[固有武将の表示上限] pbCount=%lu\r\n", pbCount);
    fprintf(f, "[読み込みの表の守り] ltLastFree=%ld ltRejectCount=%lu ltFullCount=%lu\r\n", (long)ltLastFree, ltRejectCount, ltFullCount);
    fprintf(f, "[クラッシュの守り] cgSkipCount=%lu cgDrawSkipCount=%lu cgDrawBadPtr=0x%lX cgDrawBadWho=%lu cgFxSkipCount=%lu\r\n",
            cgSkipCount, cgDrawSkipCount, cgDrawBadPtr, cgDrawBadWho, cgFxSkipCount);
    fprintf(f, "[HPの倍率] 最大HPを増やした回数 hpScaledCount=%lu(最後のTypeID=%lu) / 受けるダメージを減らした回数 hpDmgCount=%lu(最後のTypeID=%lu)\r\n",
            hpScaledCount, hpLastTypeId, hpDmgCount, hpDmgLastTypeId);
    fprintf(f, "[付け替え先なしの守り] cgRedirSkipCount=%lu 最後の場所=%lu(1=0xDB0D5A 2=0xDB0DFB 3=0xDB0E4E) / 馬の居ない記録 cgRowSkipCount=%lu\r\n",
            cgRedirSkipCount, cgRedirLastWhere, cgRowSkipCount);
    fprintf(f, "[範囲外の番号の守り] cgIdSkipCount=%lu 最後の番号=%ld 呼び出し元=+%06lX / 空の武将の記録を使った回数 cgRecSkipCount=%lu(最後の値=%ld)\r\n",
            cgIdSkipCount, (long)cgIdLastId, cgIdLastCaller, cgRecSkipCount, (long)cgRecLastId);
    fprintf(f, "[地図の切り替え] 切り替えた回数 mapToggleCount=%lu 最後の状態=%lu(0=全体 1=周辺 2=非表示)\r\n",
            mapToggleCount, mapLastMode);
    fprintf(f, "[相性のスロー] 速さを置き換えた回数(フレーム) slowCapCount=%lu 最後に見た元の倍率=%.4f(最低の速さ %.2f)\r\n",
            slowCapCount, (double)slowLastV, (double)slowMin);
    fprintf(f, "[動きのデータの守り] cgMotionSkipCount=%lu 最後の場所=+%06lX 読めなかった番地=0x%08lX\r\n",
            cgMotionSkipCount, cgMotionLastEip, cgMotionLastAddr);
    fclose(f);
}

static DWORD WINAPI worker(LPVOID arg)
{
    IMAGE_DOS_HEADER *dos;
    IMAGE_NT_HEADERS *nt;
    DWORD ts, start = GetTickCount(), lastStatus = 0;
    int i, pending, reported = 0, decrypted = 0;
    (void)arg;

    g_base = (DWORD)GetModuleHandleA(NULL);
    dos = (IMAGE_DOS_HEADER *)g_base;
    nt = (IMAGE_NT_HEADERS *)(g_base + dos->e_lfanew);
    ts = nt->FileHeader.TimeDateStamp;
    mod_log("==== mod %s 起動。ゲーム本体の先頭=0x%08lX、作成日時の印=0x%08lX ====", MOD_VERSION, g_base, ts);
    if (ts != JP_TIMESTAMP) {
        mod_log("[停止] 日本語版 1.0.5.0(0x%08X)ではないので、何も書き換えません。", JP_TIMESTAMP);
        return 0;
    }
    g_pIsBadReadPtr = (DWORD)GetProcAddress(GetModuleHandleA("kernel32.dll"), "IsBadReadPtr");
    g_imageSize = nt->OptionalHeader.SizeOfImage;
    AddVectoredExceptionHandler(0, crash_veh);   /* 最後に呼ばれる受け止め役(記録だけ) */
    for (i = 0; g_lref_table[i * 2]; i++)
        *(DWORD *)g_lref_table[i * 2] = g_base + g_lref_table[i * 2 + 1];
    load_config();
    setup_features();
    for (i = 0; i < g_nfeat; i++)
        if (!g_feat[i].enabled) { g_feat[i].done = 1; mod_log("[設定で無効] %s", g_feat[i].name); }
    if (!cfg.allyStrayZako) mod_log("[設定] 味方の野良雑魚兵 = 無(500msごとに値を0に保つ)");

    for (;;) {
        pending = 0;
        /* Steamの保護が中身を元に戻したか: どれか1つの機能の場所が全て想定どおりなら戻っている。
           戻る前の(暗号化された)バイト列は偶然 E9 のことがあるので、戻った後でしか「書き換え済み」と判断しない。
           (2026-10-23、+2108A0 の暗号化されたバイトが E9 で、動きのデータの守りを誤って中止した) */
        if (!decrypted) {
            for (i = 0; i < g_nfeat && !decrypted; i++) {
                int k, all = 1;
                if (g_feat[i].dataOnly) continue;
                if (g_feat[i].done == 1 && g_feat[i].enabled) { decrypted = 1; break; }
                for (k = 0; k < g_feat[i].nsite; k++) if (!site_matches(&g_feat[i].site[k])) all = 0;
                if (all) decrypted = 1;
            }
        }
        for (i = 0; i < g_nfeat; i++) {
            Feature *f = &g_feat[i];
            int k, all = 1, anyJmp = 0;
            if (f->done) continue;
            if (f->dataOnly && !decrypted) { pending++; continue; }   /* 数値の書き換えは、Steamの保護が戻った後に */
            for (k = 0; k < f->nsite; k++) {
                if (!site_matches(&f->site[k])) all = 0;
                if (decrypted && !f->vehOnly && site_is_jmp(&f->site[k])) anyJmp = 1;
            }
            if (all) {
                if (install_feature(f)) { f->done = 1; mod_log("[OK] %s を入れました。", f->name); }
                else pending++;
            } else if (anyJmp) {
                f->done = -1;
                mod_log("[中止] %s: 既に別のもの(CEなど)で書き換えられています。CEのmodは同時に使わないでください。", f->name);
            } else pending++;
        }
        if (!pending && !reported) { reported = 1; mod_log("全ての機能の準備が終わりました。"); }
        if (pending && !reported && GetTickCount() - start > 300000) {
            reported = 1;
            for (i = 0; i < g_nfeat; i++) {
                Feature *f = &g_feat[i];
                int k;
                if (f->done) continue;
                f->done = -1;
                for (k = 0; k < f->nsite; k++) {
                    const BYTE *p = (const BYTE *)(g_base + f->site[k].off);
                    if (!site_matches(&f->site[k]))
                        mod_log("[中止] %s: %s の命令が想定と違います(%02X %02X %02X %02X %02X %02X)。", f->name, f->site[k].name,
                             p[0], p[1], p[2], p[3], p[4], p[5]);
                }
            }
        }
        if (!cfg.allyStrayZako) keep_stray_zero();
        if (GetTickCount() - lastStatus > 30000) { lastStatus = GetTickCount(); write_status(); }
        Sleep(reported ? 500 : 50);
    }
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID reserved)
{
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) {
        char *p;
        g_self = h;
        DisableThreadLibraryCalls(h);
        InitializeCriticalSection(&g_proxyLock);
        GetModuleFileNameA(h, g_dir, MAX_PATH);
        p = strrchr(g_dir, '\\');
        if (p) *p = 0;
        CloseHandle(CreateThread(NULL, 0, worker, NULL, 0, NULL));
    }
    return TRUE;
}
