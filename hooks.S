/*
 * hooks.S  真・三國無双7 Empires(日本語版 Launch.exe 1.0.5.0)用 mod の割り込みコード
 * version.c から使う。CEで実機確認したCEAと同じ命令列(番地の参照だけを実行時に埋める表に変更)。
 *   横取りパッチ v4 (steal_patch_v4_three_tier.CEA)   ※CEAから機械変換
 *   随伴の制御 v5   (escort_control_v5.CEA)          ※CEAから機械変換
 *   固有武将の表示上限 (unique_model_boost_v5.CEA)  ※記録(pbRing)を省いたもの
 *   読み込みの表の守り (load_table_guard_v1.CEA)
 *   クラッシュの守り   (crash_guard_v3.CEA)
 * Launch.exe+X への参照は _L_X (= 実際の Launch.exe の先頭 + X、version.c が起動時に入れる)を経由する。
 * _R_xxx は割り込んだ場所の次の命令の番地(version.c が入れる)。
 */
  .intel_syntax noprefix
  .text

/* ================= 横取りパッチ v4 (steal_patch_v4_three_tier) =================
   Launch.exe+77B6B2 (E8 B9 96 FA FF 84 C0 75 E4) */
  .globl _hk_steal
_hk_steal:
st_entry:
  inc dword ptr [_hookHitCount]
  mov eax,[_L_9C39E0]
  mov eax,[eax]
  push ecx
  mov ecx,[_stThreshold]              /* 前倒し発動のしきい値(iniで変更) */
  cmp dword ptr [eax+0x7D324],ecx
  pop ecx                              /* popはフラグを変えない */
  jg st_retryGrab
  inc dword ptr [_fullCount]
  mov eax,[ebp+0x44]
  cmp eax,0x2
  jl st_originalFail
  cmp eax,0x15
  jg st_originalFail
  inc dword ptr [_allyFullCount]
  call st_stealEnemySlot
  test al,al
  jz st_stealFailedHere
  inc dword ptr [_stealSuccessCount]
  jmp st_retryGrab
st_stealFailedHere:
  inc dword ptr [_stealFailCount]
  jmp st_originalFail
st_originalFail:
  jmp dword ptr [_L_77B69F]
st_retryGrab:
  jmp dword ptr [_R_steal]
st_stealEnemySlot:
  push ecx
  push edx
  push esi
  mov edx,[_L_9C39E0]
  mov edx,[edx]
  lea esi,[edx+0x75B80]
  xor ecx,ecx
st_scanLoop1:
  cmp ecx,0x96
  jge st_scanLoop2Init
  mov eax,[esi+0x8]
  cmp eax,0x258
  jge st_skip1
  push ecx
  push edx
  imul eax,eax,0x268
  add eax,edx
  add eax,0x1B7C0
  mov eax,[eax+0x44]
  pop edx
  pop ecx
  cmp eax,0x24
  jl st_skip1
  cmp eax,0x91
  jg st_skip1
  jmp st_foundVictim
st_skip1:
  inc ecx
  add esi,0x38
  jmp st_scanLoop1
st_scanLoop2Init:
  mov edx,[_L_9C39E0]
  mov edx,[edx]
  lea esi,[edx+0x75B80]
  xor ecx,ecx
st_scanLoop2:
  cmp ecx,0x96
  jge st_scanLoop3Init
  mov eax,[esi+0x8]
  cmp eax,0x258
  jge st_skip2
  push ecx
  push edx
  imul eax,eax,0x268
  add eax,edx
  add eax,0x1B7C0
  mov eax,[eax+0x44]
  pop edx
  pop ecx
  cmp eax,0xA0
  jl st_skip2
  cmp eax,0xF9
  jg st_skip2
  jmp st_foundVictim
st_skip2:
  inc ecx
  add esi,0x38
  jmp st_scanLoop2
st_scanLoop3Init:
  mov edx,[_L_9C39E0]
  mov edx,[edx]
  lea esi,[edx+0x75B80]
  xor ecx,ecx
st_scanLoop3:
  cmp ecx,0x96
  jge st_stealFail
  mov eax,[esi+0x8]
  cmp eax,0x258
  jge st_skip3
  push ecx
  push edx
  imul eax,eax,0x268
  add eax,edx
  add eax,0x1B7C0
  mov eax,[eax+0x44]
  pop edx
  pop ecx
  cmp eax,0xFB
  jl st_skip3
  cmp eax,0x258
  jg st_skip3
  jmp st_foundVictim
st_skip3:
  inc ecx
  add esi,0x38
  jmp st_scanLoop3
st_foundVictim:
  or byte ptr [esi+0x34],0x1
  call dword ptr [_L_755CA0]
  mov al,0x1
  pop esi
  pop edx
  pop ecx
  ret
st_stealFail:
  xor al,al
  pop esi
  pop edx
  pop ecx
  ret

/* ================= 随伴の制御 v5 (escort_control_v5) =================
   Launch.exe+7891A0 / +770F34 / +6DA010 / +6DA1A4 */
  .globl _hk_ecReg
_hk_ecReg:
ec_entry:
  push ecx
  inc dword ptr [_ecHookFireCount]
  cmp byte ptr [_ecMasterEnabled],0x0
  je ec_passNoSave
  cmp eax,0x258
  jae ec_passNoSave
  push ebp
  push edx
  push ebx
  push edi
  push ecx
  mov ebp,eax
  mov ebx,[_L_9C39E0]
  mov ebx,[ebx]
  test ebx,ebx
  je ec_passRestore
  mov edx,ebp
  imul edx,edx,0x268
  add edx,ebx
  add edx,0x1B7C0
  mov edi,[edx+0x30]
  mov ecx,edi
  sub ecx,0x811
  cmp ecx,0x1B
  ja ec_notGeneric
  mov ebx,[_ecStealableMask]
  bt ebx,ecx
  jnc ec_notGeneric
  mov ecx,[edx+0x44]
  cmp ecx,[_ecAllyTypeMin]
  jl ec_checkEnemySide
  cmp ecx,[_ecAllyTypeMax]
  jg ec_checkEnemySide
  inc dword ptr [_ecAllyEscortSeenCount]
  cmp byte ptr [_ecAllyEscortEnabled],0x0
  je ec_doBlock
  jmp ec_passRestore
ec_checkEnemySide:
  cmp ecx,[_ecEnemyTypeMin]
  jl ec_checkCaptainSide
  cmp ecx,[_ecEnemyTypeMax]
  jg ec_checkCaptainSide
  inc dword ptr [_ecEnemyEscortSeenCount]
  cmp byte ptr [_ecEnemyEscortEnabled],0x0
  je ec_doBlock
ec_enemyKept:
  call ec_ecLinkedGate
  test eax,eax
  jnz ec_doLinkedBlock
  cmp byte ptr [_ecRewriteEnabled],0x0
  je ec_passRestore
  mov ecx,[_ecRewriteListCount]
  cmp ecx,0x1
  jl ec_passRestore
  cmp ecx,0x8
  jg ec_passRestore
  cmp dword ptr [_ecRngSeedInit],0x0
  jne ec_seedReady
  push edx
  rdtsc
  mov [_ecRngSeed],eax
  mov dword ptr [_ecRngSeedInit],0x1
  pop edx
ec_seedReady:
  mov eax,ebp
  mov ebx,0x9E3779B1
  imul eax,ebx
  xor eax,[_ecRngSeed]
  mov ebx,eax
  shr ebx,0xF
  xor eax,ebx
  mov ebx,0x85EBCA6B
  imul eax,ebx
  mov ebx,eax
  shr ebx,0xD
  xor eax,ebx
  push edx
  xor edx,edx
  div ecx
  mov ebx,edx
  pop edx
  lea eax,[_ecRewriteList]
  mov eax,[eax+ebx*0x4]
  cmp eax,[edx+0x30]
  je ec_passRestore
  mov [edx+0x30],eax
  inc dword ptr [_ecRewrittenCount]
  mov [_ecLastRewriteIndex],ebp
  mov [_ecLastRewriteNewOwner],eax
  jmp ec_passRestore
ec_notGeneric:
  cmp byte ptr [_ecRewriteEnabled],0x0
  je ec_memberOwnerCheck
  mov ecx,[_ecRewriteListCount]
  cmp ecx,0x1
  jl ec_memberOwnerCheck
  cmp ecx,0x8
  jg ec_memberOwnerCheck
  lea ebx,[_ecRewriteList]
ec_listScan:
  cmp edi,[ebx+ecx*0x4-0x4]
  je ec_listHit
  dec ecx
  jnz ec_listScan
  jmp ec_memberOwnerCheck
ec_listHit:
  mov ecx,[edx+0x44]
  cmp ecx,[_ecEnemyTypeMin]
  jl ec_passRestore
  cmp ecx,[_ecEnemyTypeMax]
  jg ec_passRestore
  call ec_ecLinkedGate
  test eax,eax
  jnz ec_doLinkedBlock
  jmp ec_passRestore
ec_memberOwnerCheck:
  mov ecx,[edx+0x44]
  cmp ecx,[_ecEnemyTypeMin]
  jl ec_passRestore
  cmp ecx,[_ecEnemyTypeMax]
  jg ec_passRestore
  cmp dword ptr [_ecEnemyLinkedPercent],0x64
  jae ec_passRestore
  mov ecx,edi
  sub ecx,0x807
  cmp ecx,0x1D
  ja ec_moNotProtected
  mov ebx,[_ecProtectedMask]
  bt ebx,ecx
  jc ec_passRestore
ec_moNotProtected:
  mov ecx,[esp+0x1C]
  cmp ecx,0x1
  jl ec_passRestore
  cmp ecx,0x5F
  jg ec_passRestore
  mov ebx,[edx+0x2C]
  cmp ebx,0x3FF
  ja ec_passRestore
  imul ebx,ebx,0x60
  add ebx,ecx
  mov eax,[_L_9C5DB0]
  mov ebx,[eax+ebx*0x4]
  sub ebx,0x811
  cmp ebx,0x1B
  ja ec_passRestore
  mov eax,[_ecStealableMask]
  bt eax,ebx
  jnc ec_passRestore
  inc dword ptr [_ecMemberOwnerPathCount]
  mov [_ecLastMemberOwnerIndex],ebp
  mov [_ecLastMemberOwnerEntryOwner],edi
  call ec_ecLinkedGate
  test eax,eax
  jnz ec_doLinkedBlock
  jmp ec_passRestore
ec_doLinkedBlock:
  inc dword ptr [_ecLinkedBlockedCount]
  mov [_ecLastLinkedBlockedIndex],ebp
  jmp ec_blockCommon
ec_doBlock:
  inc dword ptr [_ecBlockedCount]
  mov [_ecLastBlockedIndex],ebp
  mov [_ecLastBlockedOwner],edi
  mov ecx,[edx+0x44]
  mov [_ecLastBlockedTypeId],ecx
ec_blockCommon:
  pop ecx
  pop edi
  pop ebx
  pop edx
  pop ebp
  test esi,esi
  jz ec_skipSetFlag
  mov byte ptr [esi],0x1
ec_skipSetFlag:
  mov eax,0x96
  pop ecx
  ret
ec_passRestore:
  mov eax,ebp
  pop ecx
  pop edi
  pop ebx
  pop edx
  pop ebp
ec_passNoSave:
  cmp eax,0x258
  jmp dword ptr [_R_ecReg]
ec_checkCaptainSide:
  cmp ecx,[_ecBaseTypeMin]
  jl ec_ccRoam
  cmp ecx,[_ecBaseTypeMax]
  jg ec_ccRoam
  inc dword ptr [_ecBaseEscortSeenCount]
  mov [_ecLastBaseEscortTypeId],ecx
  cmp byte ptr [_ecBaseRewriteEnabled],0x0
  je ec_passRestore
  lea ebx,[_ecBaseRewriteList]
  mov ecx,[_ecBaseRewriteListCount]
  call ec_ecRewriteFrom
  add [_ecBaseRewrittenCount],eax
  jmp ec_passRestore
ec_ccRoam:
  cmp ecx,[_ecRoamTypeMin]
  jl ec_passRestore
  cmp ecx,[_ecRoamTypeMax]
  jg ec_passRestore
  inc dword ptr [_ecRoamEscortSeenCount]
  mov [_ecLastRoamEscortTypeId],ecx
  cmp byte ptr [_ecRoamRewriteEnabled],0x0
  je ec_passRestore
  lea ebx,[_ecRoamRewriteList]
  mov ecx,[_ecRoamRewriteListCount]
  call ec_ecRewriteFrom
  add [_ecRoamRewrittenCount],eax
  jmp ec_passRestore
ec_ecRewriteFrom:
  cmp ecx,0x1
  jl ec_rfNone
  cmp ecx,0x8
  jg ec_rfNone
  push edx
  call ec_ecEnsureSeed
  mov eax,ebp
  imul eax,eax,0x9E3779B1
  xor eax,[_ecRngSeed]
  mov edx,eax
  shr edx,0xF
  xor eax,edx
  imul eax,eax,0x85EBCA6B
  mov edx,eax
  shr edx,0xD
  xor eax,edx
  xor edx,edx
  div ecx
  mov eax,[ebx+edx*0x4]
  pop edx
  cmp eax,[edx+0x30]
  je ec_rfNone
  mov [edx+0x30],eax
  mov eax,0x1
  ret
ec_rfNone:
  xor eax,eax
  ret
ec_ecEnsureSeed:
  cmp dword ptr [_ecRngSeedInit],0x0
  jne ec_esReady
  rdtsc
  mov [_ecRngSeed],eax
  mov dword ptr [_ecRngSeedInit],0x1
ec_esReady:
  ret
  .globl _hk_ecMember
_hk_ecMember:
ec_memberHook:
  shl eax,2
  add eax,[_L_9C5DB0]
  mov eax,[eax]
  cmp byte ptr [_ecMasterEnabled],0x0
  je ec_mhExit
  cmp byte ptr [_ecMemberRewriteEnabled],0x0
  je ec_mhExit
  test esi,esi
  je ec_mhExit
  push ebx
  push ecx
  push edx
  push edi
  mov edx,eax
  sub edx,0x811
  cmp edx,0x1B
  ja ec_mhDone
  mov ebx,[_ecStealableMask]
  bt ebx,edx
  jnc ec_mhDone
  mov edx,[esi+0x44]
  cmp edx,[_ecEnemyTypeMin]
  jl ec_mhBase
  cmp edx,[_ecEnemyTypeMax]
  jg ec_mhBase
  cmp byte ptr [_ecRewriteEnabled],0x0
  je ec_mhDone
  lea ebx,[_ecRewriteList]
  mov edi,[_ecRewriteListCount]
  jmp ec_mhHave
ec_mhBase:
  cmp edx,[_ecBaseTypeMin]
  jl ec_mhRoam
  cmp edx,[_ecBaseTypeMax]
  jg ec_mhRoam
  cmp byte ptr [_ecBaseRewriteEnabled],0x0
  je ec_mhDone
  lea ebx,[_ecBaseRewriteList]
  mov edi,[_ecBaseRewriteListCount]
  jmp ec_mhHave
ec_mhRoam:
  cmp edx,[_ecRoamTypeMin]
  jl ec_mhDone
  cmp edx,[_ecRoamTypeMax]
  jg ec_mhDone
  cmp byte ptr [_ecRoamRewriteEnabled],0x0
  je ec_mhDone
  lea ebx,[_ecRoamRewriteList]
  mov edi,[_ecRoamRewriteListCount]
ec_mhHave:
  cmp edi,0x1
  jl ec_mhDone
  cmp edi,0x8
  jg ec_mhDone
  mov edx,[esi+0x30]
  sub edx,0x811
  cmp edx,0x1B
  ja ec_mhList
  mov ecx,[_ecStealableMask]
  bt ecx,edx
  jc ec_mhOwnerOk
ec_mhList:
  mov edx,[esi+0x30]
  mov ecx,edi
ec_mhListScan:
  cmp edx,[ebx+ecx*0x4-0x4]
  je ec_mhOwnerOk
  dec ecx
  jnz ec_mhListScan
  jmp ec_mhDone
ec_mhOwnerOk:
  call ec_ecEnsureSeed
  mov eax,[ebp+0x08]
  imul eax,eax,0x9E3779B1
  mov edx,[esp+0x08]
  imul edx,edx,0x85EBCA6B
  xor eax,edx
  xor eax,[_ecRngSeed]
  mov edx,eax
  shr edx,0xF
  xor eax,edx
  imul eax,eax,0x27D4EB2F
  mov edx,eax
  shr edx,0xD
  xor eax,edx
  xor edx,edx
  div edi
  mov eax,[ebx+edx*0x4]
  inc dword ptr [_ecMemberRewrittenCount]
  mov edx,[ebp+0x08]
  mov [_ecLastMemberRewriteIndex],edx
  mov [_ecLastMemberRewriteOwner],eax
ec_mhDone:
  pop edi
  pop edx
  pop ecx
  pop ebx
ec_mhExit:
  jmp dword ptr [_R_ecMember]
ec_ecLinkedGate:
  mov ecx,[esp+0x20]
  test ecx,ecx
  jle ec_gateAllow
  inc dword ptr [_ecEnemyMemberSeenCount]
  mov [_ecLastMemberNo],ecx
  mov ebx,[_ecEnemyLinkedPercent]
  cmp ebx,0x64
  jae ec_gateAllow
  push edx
  cmp dword ptr [_ecRngSeedInit],0x0
  jne ec_gateSeedReady
  push ecx
  rdtsc
  mov [_ecRngSeed],eax
  mov dword ptr [_ecRngSeedInit],0x1
  pop ecx
ec_gateSeedReady:
  mov eax,ebp
  imul eax,eax,0x9E3779B1
  mov edx,ecx
  imul edx,edx,0x85EBCA6B
  xor eax,edx
  xor eax,[_ecRngSeed]
  mov edx,eax
  shr edx,0xF
  xor eax,edx
  imul eax,eax,0x27D4EB2F
  mov edx,eax
  shr edx,0xD
  xor eax,edx
  xor edx,edx
  mov ecx,0x64
  div ecx
  xor eax,eax
  cmp edx,ebx
  jb ec_gateDone
  mov eax,0x1
ec_gateDone:
  pop edx
  ret
ec_gateAllow:
  xor eax,eax
  ret
  .globl _hk_ecBudgetSkip
_hk_ecBudgetSkip:
ec_budgetSkipHook:
  cmp byte ptr [_ecMasterEnabled],0x0
  je ec_bsReplicate
  cmp byte ptr [_ecBudgetFilterEnabled],0x0
  je ec_bsReplicate
  push eax
  push ecx
  push edx
  cmp esi,0x258
  jae ec_bsRestore
  mov eax,[_L_9C39E0]
  mov eax,[eax]
  test eax,eax
  je ec_bsRestore
  imul edx,esi,0x268
  lea edx,[edx+eax+0x1B7C0]
  mov eax,[edx+0x30]
  lea ecx,[eax-0x811]
  cmp ecx,0x1B
  ja ec_bsRestore
  mov eax,[_ecStealableMask]
  bt eax,ecx
  jnc ec_bsRestore
  mov ecx,[edx+0x44]
  cmp ecx,[_ecAllyTypeMin]
  jl ec_bsChkEnemy
  cmp ecx,[_ecAllyTypeMax]
  jg ec_bsChkEnemy
  cmp byte ptr [_ecAllyEscortEnabled],0x0
  je ec_bsSkip
  jmp ec_bsRestore
ec_bsChkEnemy:
  cmp ecx,[_ecEnemyTypeMin]
  jl ec_bsRestore
  cmp ecx,[_ecEnemyTypeMax]
  jg ec_bsRestore
  cmp byte ptr [_ecEnemyEscortEnabled],0x0
  jne ec_bsRestore
ec_bsSkip:
  inc dword ptr [_ecBudgetSkipCount]
  mov [_ecLastBudgetSkipIndex],esi
  pop edx
  pop ecx
  pop eax
  jmp dword ptr [_L_6DA228]
ec_bsRestore:
  pop edx
  pop ecx
  pop eax
ec_bsReplicate:
  mov eax,[esp+0x30]
  cmp dword ptr [eax+esi*0x8+0x04],0x0
  jmp dword ptr [_R_ecBudgetSkip]
  .globl _hk_ecBudgetMember
_hk_ecBudgetMember:
ec_budgetMemberHook:
  pushad
  cmp byte ptr [_ecMasterEnabled],0x0
  je ec_bmDone
  cmp byte ptr [_ecBudgetFilterEnabled],0x0
  je ec_bmDone
  cmp dword ptr [_ecEnemyLinkedPercent],0x64
  jae ec_bmDone
  mov ebp,esi
  cmp ebp,0x258
  jae ec_bmDone
  mov eax,[_L_9C39E0]
  mov eax,[eax]
  test eax,eax
  je ec_bmDone
  imul edx,ebp,0x268
  lea edx,[edx+eax+0x1B7C0]
  mov eax,[edx+0x44]
  cmp eax,[_ecEnemyTypeMin]
  jl ec_bmDone
  cmp eax,[_ecEnemyTypeMax]
  jg ec_bmDone
  mov eax,[edx+0x30]
  lea ecx,[eax-0x811]
  cmp ecx,0x1B
  ja ec_bmNotGeneric
  mov ebx,[_ecStealableMask]
  bt ebx,ecx
  jc ec_bmMode1
ec_bmNotGeneric:
  cmp byte ptr [_ecRewriteEnabled],0x0
  je ec_bmMode2
  mov ecx,[_ecRewriteListCount]
  cmp ecx,0x1
  jl ec_bmMode2
  cmp ecx,0x8
  jg ec_bmMode2
  lea ebx,[_ecRewriteList]
ec_bmListScan:
  cmp eax,[ebx+ecx*0x4-0x4]
  je ec_bmMode1
  dec ecx
  jnz ec_bmListScan
ec_bmMode2:
  lea ecx,[eax-0x807]
  cmp ecx,0x1D
  ja ec_bmHaveMode2
  mov ebx,[_ecProtectedMask]
  bt ebx,ecx
  jc ec_bmDone
ec_bmHaveMode2:
  mov edi,0x2
  jmp ec_bmHaveMode
ec_bmMode1:
  mov edi,0x1
ec_bmHaveMode:
  mov esi,[esp+0x14]
  mov ecx,0x1
ec_bmLoop:
  bt esi,ecx
  jnc ec_bmNext
  cmp edi,0x1
  je ec_bmGate
  mov ebx,[edx+0x2C]
  cmp ebx,0x3FF
  ja ec_bmNext
  imul ebx,ebx,0x60
  add ebx,ecx
  mov eax,[_L_9C5DB0]
  mov ebx,[eax+ebx*0x4]
  sub ebx,0x811
  cmp ebx,0x1B
  ja ec_bmNext
  mov eax,[_ecStealableMask]
  bt eax,ebx
  jnc ec_bmNext
ec_bmGate:
  call ec_ecGateCore
  test eax,eax
  je ec_bmNext
  btr esi,ecx
  inc dword ptr [_ecBudgetMemberClearedCount]
ec_bmNext:
  inc ecx
  cmp ecx,0x1E
  jb ec_bmLoop
  mov [esp+0x14],esi
ec_bmDone:
  popad
  mov eax,edx
  shr eax,0x1
  and eax,0x55555555
  jmp dword ptr [_R_ecBudgetMember]
ec_ecGateCore:
  push ebx
  push ecx
  push edx
  mov ebx,[_ecEnemyLinkedPercent]
  cmp ebx,0x64
  jae ec_gcAllow
  call ec_ecEnsureSeed
  mov ecx,[esp+0x04]
  mov eax,ebp
  imul eax,eax,0x9E3779B1
  mov edx,ecx
  imul edx,edx,0x85EBCA6B
  xor eax,edx
  xor eax,[_ecRngSeed]
  mov edx,eax
  shr edx,0xF
  xor eax,edx
  imul eax,eax,0x27D4EB2F
  mov edx,eax
  shr edx,0xD
  xor eax,edx
  xor edx,edx
  mov ecx,0x64
  div ecx
  xor eax,eax
  cmp edx,ebx
  jb ec_gcDone
  mov eax,0x1
  jmp ec_gcDone
ec_gcAllow:
  xor eax,eax
ec_gcDone:
  pop edx
  pop ecx
  pop ebx
  ret

/* ================= 固有武将の表示上限 (unique_model_boost_v5) =================
   Launch.exe+659AF0 (FUN_00e19af0 の入口 55 8B EC 83 E4 F8) */
  .globl _hk_umb
_hk_umb:
  pushfd
  pushad                               /* 以降、元の[esp+8](param_3) = [esp+2C] */
  mov edi,[_g_base]                    /* edi = Launch.exe の先頭 */
  mov eax,[edi+0x9C3A74]               /* A */
  mov edx,[_pbToA]
  test edx,edx
  je umb_no
  cmp eax,6
  jb umb_no
  cmp eax,0x0A
  ja umb_no
  cmp eax,edx
  jae umb_no
  lea ecx,[eax*4]
  cmp [edi+0x9C3AB0],ecx               /* B = A*4 か */
  jne umb_no
  lea ecx,[eax+eax*2]
  add ecx,0x15
  add ecx,ecx
  cmp [edi+0x9C3ACC],ecx               /* N = (A*3+21)*2 か */
  jne umb_no
  mov [edi+0x9C3A74],edx
  cmp [edi+0x9C3A78],eax
  jne umb_a1
  mov [edi+0x9C3A78],edx
umb_a1:
  mov ecx,[_pbToB]
  test ecx,ecx
  jne umb_a2
  lea ecx,[edx*4]
umb_a2:
  mov [edi+0x9C3AB0],ecx
  mov ebx,eax
  shl ebx,2
  cmp [edi+0x9C3AB8],ebx
  jne umb_a3
  mov [edi+0x9C3AB8],ecx
umb_a3:
  mov ecx,[_pbToN]
  test ecx,ecx
  jne umb_a4
  lea ecx,[edx+edx*2]
  add ecx,0x15
  add ecx,ecx
umb_a4:
  mov [edi+0x9C3ACC],ecx
  cmp [esp+0x2C],eax
  jne umb_no
  mov [esp+0x2C],edx
umb_no:
  inc dword ptr [_pbCount]
  popad
  popfd
  push ebp                             /* 元の命令 */
  mov ebp,esp                          /* 元の命令 */
  .byte 0x83,0xE4,0xF8                 /* 元の命令 and esp,-8 */
  jmp dword ptr [_R_umb]

/* ================= 読み込みの表の守り (load_table_guard_v1) ================= */
/* (1) Launch.exe+657480 (FUN_00e17480 の入口) */
  .globl _hk_ltWait
_hk_ltWait:
  push eax
  push edx
  push esi
  lea esi,[ecx+0x78]
  xor edx,edx
  xor eax,eax
lt_loop:
  cmp dword ptr [esi],0
  jge lt_used
  inc edx
lt_used:
  add esi,0x10
  inc eax
  cmp eax,0x5A
  jb lt_loop
  mov [_ltLastFree],edx
  cmp edx,[_ltMinFree]
  pop esi
  pop edx
  pop eax
  jae lt_ok
  inc dword ptr [_ltRejectCount]
  xor al,al
  ret 4
lt_ok:
  push ebp
  mov ebp,esp
  .byte 0x83,0xE4,0xF8
  jmp dword ptr [_R_ltWait]
/* (2) Launch.exe+5D9ADB (cmp ebp,5A / jb) */
  .globl _hk_ltFull
_hk_ltFull:
  cmp ebp,0x5A
  jae lt_full
  jmp dword ptr [_L_5D9AD1]            /* 元の jb の行き先(まだ探す) */
lt_full:
  inc dword ptr [_ltFullCount]
  jmp dword ptr [_L_5D9AC3]            /* 0で返る終わり方 */

/* ================= クラッシュの守り (crash_guard_v3) ================= */
/* (1) Launch.exe+7FB6B8 0での割り算 */
  .globl _hk_cgDiv
_hk_cgDiv:
  fidiv dword ptr [esp+0x10]
  test esi,esi
  je cg_skip
  div esi
  jmp dword ptr [_R_cgDiv]
cg_skip:
  inc dword ptr [_cgSkipCount]
  jmp dword ptr [_R_cgDiv]

/* 物体の確かめ: ECX=物体。EAX=1なら良い、0なら駄目。EAX/ECX/EDXを壊す */
cg_checkObj:
  cmp ecx,0x10000
  jb cg_bad
  push ecx
  push 4
  push ecx
  call dword ptr [_g_pIsBadReadPtr]
  pop ecx
  test eax,eax
  jne cg_bad
  mov eax,[ecx]
  mov edx,[_g_base]
  sub eax,edx
  jb cg_bad
  mov ecx,[edx+0x3C]
  mov ecx,[edx+ecx+0x50]
  cmp eax,ecx
  jae cg_bad
  mov eax,1
  ret
cg_bad:
  xor eax,eax
  ret

/* (2a) Launch.exe+5F0E59 */
  .globl _hk_cgDraw
_hk_cgDraw:
  mov ecx,ebx
  call cg_checkObj
  test eax,eax
  je cg_ebxBad
  mov ecx,esi
  call cg_checkObj
  test eax,eax
  je cg_esiBad
  mov eax,[ebx]                        /* 元の命令 */
  mov edx,[eax+0x08]                   /* 元の命令 */
  jmp dword ptr [_R_cgDraw]
cg_ebxBad:
  mov [_cgDrawBadPtr],ebx
  mov dword ptr [_cgDrawBadWho],1
  jmp cg_drawSkip
cg_esiBad:
  mov [_cgDrawBadPtr],esi
  mov dword ptr [_cgDrawBadWho],2
cg_drawSkip:
  inc dword ptr [_cgDrawSkipCount]
  jmp dword ptr [_L_5F0FC8]            /* LAB_00db0fc8(関数の終わり) */

/* (2b) Launch.exe+5F0E6F */
  .globl _hk_cgPart
_hk_cgPart:
  mov ecx,[esp+0x0C]                   /* 元の命令 */
  mov [esp+0x10],eax                   /* 元の命令 */
  test ecx,ecx
  je cg_part2
  test eax,eax
  jne cg_partOk
  mov [_cgDrawBadPtr],eax
  mov dword ptr [_cgDrawBadWho],4
  jmp cg_drawSkip
cg_partOk:
  jmp dword ptr [_R_cgPart]
cg_part2:
  mov [_cgDrawBadPtr],ecx
  mov dword ptr [_cgDrawBadWho],3
  jmp cg_drawSkip

/* (3) Launch.exe+625030 (FUN_00de5030 の入口) */
  .globl _hk_cgFx
_hk_cgFx:
  mov eax,[esp+0x04]
  test eax,eax
  je cg_fxSkip
  mov eax,[eax]
  test eax,eax
  je cg_fxSkip
  cmp dword ptr [eax+0x04],0
  je cg_fxSkip
  push ebp
  mov ebp,esp
  .byte 0x83,0xE4,0xF8
  jmp dword ptr [_R_cgFx]
cg_fxSkip:
  inc dword ptr [_cgFxSkipCount]
  xor eax,eax
  ret 0x0C

/* ================= 動きのデータの守り(version.c の例外の受け止め役から来る) =================
   FUN_009d08a0 の途中で不正アクセスが起きた時の抜け方。FLD1 で積んだ 1 を下ろし、積んだレジスタを戻して ret 0C。 */
  .globl _cg_motionExitA
_cg_motionExitA:                       /* 0x9D08A9/AC/AF: push ebp, push ebx の後 */
  fstp st(0)
  pop ebx
  pop ebp
  ret 0x0C
  .globl _cg_motionExitB
_cg_motionExitB:                       /* 0x9D08C2: さらに push esi, push edi の後 */
  fstp st(0)
  pop edi
  pop esi
  pop ebx
  pop ebp
  ret 0x0C

/* ================= 範囲外の番号の守り =================
   Launch.exe+616D74 (FUN_00dd6d30 の中、33 C0 8B 40 28 C3 = xor eax,eax / mov eax,[eax+28] / ret)。
   この時点で EAX = 範囲外の番号、[ESP] = 呼び出し元。-1 を返す。 */
  .globl _hk_cgBadId
_hk_cgBadId:
  inc dword ptr [_cgIdSkipCount]
  mov [_cgIdLastId],eax
  mov eax,[esp]
  sub eax,[_g_base]
  mov [_cgIdLastCaller],eax
  or eax,0xFFFFFFFF
  ret

/* Launch.exe+68AAB1 (FUN_00e4a080 の中、3D 37 09 00 00 = cmp eax,937)。EAX = FUN_00dd6d30 の結果(武将の番号)。
   範囲内なら元どおり(フラグを保ったまま +68AAB6 の ja へ)。範囲外なら「空の武将の記録」を EAX にして
   +68AAC7(movsx eax,word ptr [eax+0E])へ進む。 */
  .globl _hk_cgCharRec
_hk_cgCharRec:
  cmp eax,0x937                        /* 元の命令 */
  jbe cg_recOk
  inc dword ptr [_cgRecSkipCount]
  mov [_cgRecLastId],eax
  mov eax,offset _cg_dummyRecord
  jmp dword ptr [_L_68AAC7]
cg_recOk:
  jmp dword ptr [_R_cgCharRec]         /* ja は不成立のまま */

/* ================= 付け替え先なしの守り =================
   FUN_00dab7e0(付け替え先の行を探す)の直後の `mov 物体,[表+行*4+DC]` に割り込み、行が負(-1=見つからない)なら
   付け替えをやめて元の物体のまま続ける。後ろの命令はフラグを使わないので、test で変えてよい。 */
  .globl _hk_cgRedir1
_hk_cgRedir1:                          /* +5F0D5A (FUN_00db0d10) EDI=元の物体 */
  test eax,eax
  js cg_rd1Skip
  mov edi,[esi+eax*4+0xDC]             /* 元の命令 */
  jmp dword ptr [_R_cgRedir1]
cg_rd1Skip:
  inc dword ptr [_cgRedirSkipCount]
  mov dword ptr [_cgRedirLastWhere],1
  jmp dword ptr [_R_cgRedir1]
  .globl _hk_cgRedir2
_hk_cgRedir2:                          /* +5F0DFB (FUN_00db0d80 の1つ目) ESI=元の物体 */
  test eax,eax
  js cg_rd2Skip
  mov esi,[edx+eax*4+0xDC]             /* 元の命令 */
  jmp dword ptr [_R_cgRedir2]
cg_rd2Skip:
  inc dword ptr [_cgRedirSkipCount]
  mov dword ptr [_cgRedirLastWhere],2
  jmp dword ptr [_R_cgRedir2]
  .globl _hk_cgRedir3
_hk_cgRedir3:                          /* +5F0E4E (FUN_00db0d80 の2つ目) EBX=元の物体 */
  test eax,eax
  js cg_rd3Skip
  mov ebx,[edx+eax*4+0xDC]             /* 元の命令 */
  jmp dword ptr [_R_cgRedir3]
cg_rd3Skip:
  inc dword ptr [_cgRedirSkipCount]
  mov dword ptr [_cgRedirLastWhere],3
  jmp dword ptr [_R_cgRedir3]

/* Launch.exe+666D95 (FUN_00e26d70 の中、8B AC 88 DC 00 00 00 = mov ebp,[eax+ecx*4+DC])。ECX = 記録の +0xC の行。
   行が負(-1=居ない馬)なら EBP=0 にする → 直後の test ebp,ebp / jz でゲーム自身の「物体が無い」流れへ。 */
  .globl _hk_cgRow
_hk_cgRow:
  test ecx,ecx
  js cg_rowSkip
  mov ebp,[eax+ecx*4+0xDC]             /* 元の命令 */
  jmp dword ptr [_R_cgRow]
cg_rowSkip:
  inc dword ptr [_cgRowSkipCount]
  xor ebp,ebp
  jmp dword ptr [_R_cgRow]

/* ================= 地図の切り替え(全体 → 周辺 → 非表示) =================
   Launch.exe+75349A (FUN_00f13400 の LAB_00f1349a、8B 46 08 = mov eax,[esi+8])。ESI = 地図の物体。
   元は「(前の状態 + 1) を 2 で割った余り」で 0(全体)と 1(周辺)を交互にするだけ。
   ここを「(前の状態 + 1) が _mapModeCount(3) 以上なら 0」にして、2(ゲームが元から持つ「非表示」)も順番に入れる。
   +7534B0(mov edx,[DAT_011839e0])へ戻る。間の命令(0xF134A3〜0xF134AF)は通らない。 */
  .globl _hk_mapToggle
_hk_mapToggle:
  mov eax,[esi+8]
  inc eax
  cmp eax,[_mapModeCount]
  jb map_tgOk
  xor eax,eax
map_tgOk:
  mov [esi+0xC],eax                    /* 元の命令 */
  mov [esi+8],eax                      /* 元の命令 */
  mov [_mapLastMode],eax
  inc dword ptr [_mapToggleCount]
  jmp dword ptr [_R_mapToggle]

/* ================= 相性のスローの速さ =================
   Launch.exe+6545F7 (FUN_00e14400 の中、D8 4C 24 10 / D9 5C 24 10 = fmul [esp+10] / fstp [esp+10])。
   ここは「状態 0x0C の効果」(敵の攻撃のスロー・反撃の演出のスロー)がある時だけ通り、ST0 = その倍率(スローの速さ)。
   倍率が _slowMin より小さい時は _slowMin に置き換える(スロー中でも最低この速さにする)。+6545FF へ戻る。 */
  .globl _hk_slowCap
_hk_slowCap:
  fst dword ptr [_slowLastV]
  fld dword ptr [_slowMin]             /* st0 = 最低の速さ、st1 = 元の倍率 */
  fcomi st(0),st(1)
  jbe sc_keep                          /* 最低 <= 元(または比べられない)なら元のまま */
  fstp st(1)                           /* st0 = 最低の速さ */
  inc dword ptr [_slowCapCount]
  jmp sc_mul
sc_keep:
  fstp st(0)                           /* st0 = 元の倍率 */
sc_mul:
  fmul dword ptr [esp+0x10]            /* 元の命令 */
  fstp dword ptr [esp+0x10]            /* 元の命令 */
  jmp dword ptr [_R_slowCap]

/* ================= HPの倍率 =================
   Launch.exe+7710A8 (FUN_00f30e90 の中、FUN_00f27a20 の直後。80 7C 24 1F 00 = cmp byte ptr [esp+1F],0)。
   ESI = 600番の行、[esp+1F] = 本体なら1、[esp+48] = 最大HP(小数)。 */
  .globl _hk_hpScale
_hk_hpScale:
  push eax
  push ecx                             /* 以降、元の [esp+X] は [esp+X+8] */
  cmp byte ptr [_hpBodyOnly],0
  je hp_any
  cmp byte ptr [esp+0x27],0            /* 本体か(元の [esp+1F]) */
  je hp_done
hp_any:
  test esi,esi
  je hp_done
  mov eax,[esi+0x44]                   /* TypeID */
  cmp eax,0x100
  jae hp_done
  mov ecx,[_hpScaleTable+eax*4]
  test ecx,ecx                         /* 0 = 対象外 */
  je hp_done
  fld dword ptr [esp+0x50]             /* 元の [esp+48] = 最大HP */
  fmul dword ptr [_hpScaleTable+eax*4]
  fstp dword ptr [esp+0x50]
  inc dword ptr [_hpScaledCount]
  mov [_hpLastTypeId],eax
hp_done:
  pop ecx
  pop eax
  cmp byte ptr [esp+0x1F],0            /* 元の命令(直後の jz のためのフラグ) */
  jmp dword ptr [_R_hpScale]

/* ================= HPの倍率(受けるダメージを減らす) =================
   Launch.exe+6E78E0 (FUN_00ea78e0 の入口、83 EC 08 3D 96 00 00 00 = sub esp,8 / cmp eax,96)。
   EAX = 参加者の番号(0〜149)、[esp+4] = HPの増減量(小数、ダメージなら負)。 */
  .globl _hk_hpDmg
_hk_hpDmg:
  push ecx
  push edx                             /* 以降、元の [esp+4] は [esp+0C] */
  cmp eax,0x96
  jae hd_done
  test dword ptr [esp+0x0C],0x80000000 /* 負(ダメージ)の時だけ */
  je hd_done
  mov edx,[_L_9C39E0]
  mov edx,[edx]
  test edx,edx
  je hd_done
  lea ecx,[eax*8]
  sub ecx,eax
  lea ecx,[edx+ecx*8+0x75B80]          /* 参加者の行 */
  cmp byte ptr [_hpBodyOnly],0
  je hd_any
  cmp dword ptr [ecx+0x0C],0           /* メンバー番号 0 = 本体 */
  jne hd_done
hd_any:
  mov ecx,[ecx+0x08]                   /* 600番 */
  cmp ecx,600
  jae hd_done
  imul ecx,ecx,0x268
  mov ecx,[edx+ecx+0x1B804]            /* 600番の行 +0x44 = TypeID(0x1B7C0+0x44) */
  cmp ecx,0x100
  jae hd_done
  cmp dword ptr [_hpDmgTable+ecx*4],0  /* 0 = 対象外、それ以外は 1/倍率 */
  je hd_done
  fld dword ptr [esp+0x0C]
  fmul dword ptr [_hpDmgTable+ecx*4]
  fstp dword ptr [esp+0x0C]
  inc dword ptr [_hpDmgCount]
  mov [_hpDmgLastTypeId],ecx
hd_done:
  pop edx
  pop ecx
  sub esp,8                            /* 元の命令 */
  cmp eax,0x96                         /* 元の命令(直後の jnc のためのフラグ) */
  jmp dword ptr [_R_hpDmg]

  .data
  .p2align 2
/* ---- version.c が起動時に入れる値 ---- */
  .globl _g_base
_g_base: .long 0
  .globl _g_pIsBadReadPtr
_g_pIsBadReadPtr: .long 0
  .globl _L_6DA228
_L_6DA228: .long 0
  .globl _L_755CA0
_L_755CA0: .long 0
  .globl _L_77B69F
_L_77B69F: .long 0
  .globl _L_9C39E0
_L_9C39E0: .long 0
  .globl _L_9C5DB0
_L_9C5DB0: .long 0
  .globl _L_5D9AD1
_L_5D9AD1: .long 0
  .globl _L_5D9AC3
_L_5D9AC3: .long 0
  .globl _L_5F0FC8
_L_5F0FC8: .long 0
  .globl _L_68AAC7
_L_68AAC7: .long 0
  .globl _R_steal
_R_steal: .long 0
  .globl _R_ecReg
_R_ecReg: .long 0
  .globl _R_ecMember
_R_ecMember: .long 0
  .globl _R_ecBudgetSkip
_R_ecBudgetSkip: .long 0
  .globl _R_ecBudgetMember
_R_ecBudgetMember: .long 0
  .globl _R_umb
_R_umb: .long 0
  .globl _R_ltWait
_R_ltWait: .long 0
  .globl _R_cgDiv
_R_cgDiv: .long 0
  .globl _R_cgDraw
_R_cgDraw: .long 0
  .globl _R_cgPart
_R_cgPart: .long 0
  .globl _R_cgFx
_R_cgFx: .long 0
  .globl _R_cgCharRec
_R_cgCharRec: .long 0
  .globl _R_hpDmg
_R_hpDmg: .long 0
  .globl _R_hpScale
_R_hpScale: .long 0
  .globl _R_cgRow
_R_cgRow: .long 0
  .globl _R_cgRedir1
_R_cgRedir1: .long 0
  .globl _R_cgRedir2
_R_cgRedir2: .long 0
  .globl _R_cgRedir3
_R_cgRedir3: .long 0
  .globl _R_slowCap
_R_slowCap: .long 0
  .globl _R_mapToggle
_R_mapToggle: .long 0
/* Launch.exe+X の表: (変数, X) の組。0で終わり */
  .globl _g_lref_table
_g_lref_table:
  .long _L_6DA228, 0x6DA228
  .long _L_755CA0, 0x755CA0
  .long _L_77B69F, 0x77B69F
  .long _L_9C39E0, 0x9C39E0
  .long _L_9C5DB0, 0x9C5DB0
  .long _L_5D9AD1, 0x5D9AD1
  .long _L_5D9AC3, 0x5D9AC3
  .long _L_5F0FC8, 0x5F0FC8
  .long _L_68AAC7, 0x68AAC7
  .long 0, 0
/* ---- 設定と記録 ---- */
  .globl _stThreshold
_stThreshold: .long 3
  .globl _pbToA
_pbToA: .long 0x0E
  .globl _pbToB
_pbToB: .long 0
  .globl _pbToN
_pbToN: .long 0
  .globl _pbCount
_pbCount: .long 0
  .globl _ltLastFree
_ltLastFree: .long 0xFFFFFFFF
  .globl _ltRejectCount
_ltRejectCount: .long 0
  .globl _ltFullCount
_ltFullCount: .long 0
  .globl _ltMinFree
_ltMinFree: .long 6
  .globl _cgSkipCount
_cgSkipCount: .long 0
  .globl _cgDrawSkipCount
_cgDrawSkipCount: .long 0
  .globl _cgDrawBadPtr
_cgDrawBadPtr: .long 0
  .globl _cgDrawBadWho
_cgDrawBadWho: .long 0
  .p2align 3
  .globl _msgFrameX
_msgFrameX: .double 68.0
  .globl _msgTalkY1
_msgTalkY1: .double 296.0
  .globl _msgTalkY2
_msgTalkY2: .double 371.0
  .globl _msgFrameY1
_msgFrameY1: .float 296.0
  .globl _msgFrameY2
_msgFrameY2: .float 371.0
  .globl _msgTalkX
_msgTalkX: .float 223.0
  .globl _msgBandY
_msgBandY: .float 348.0
  .globl _msgBandY2
_msgBandY2: .float 336.0
  .globl _msgFxY
_msgFxY: .float 360.0
  .globl _msgFxY2
_msgFxY2: .float 348.0
  .globl _hpBodyOnly
_hpBodyOnly: .byte 1
  .p2align 2
  .globl _hpScaledCount
_hpScaledCount: .long 0
  .globl _hpLastTypeId
_hpLastTypeId: .long 0
  .globl _hpDmgCount
_hpDmgCount: .long 0
  .globl _hpDmgLastTypeId
_hpDmgLastTypeId: .long 0
  .globl _hpDmgTable
_hpDmgTable: .space 0x400
  .globl _hpScaleTable
_hpScaleTable: .space 0x400
  .globl _cgRowSkipCount
_cgRowSkipCount: .long 0
  .globl _slowMin
_slowMin: .float 0.85
  .globl _slowLastV
_slowLastV: .float 1.0
  .globl _slowCapCount
_slowCapCount: .long 0
  .globl _mapModeCount
_mapModeCount: .long 3
  .globl _mapToggleCount
_mapToggleCount: .long 0
  .globl _mapLastMode
_mapLastMode: .long 0
  .globl _cgRedirSkipCount
_cgRedirSkipCount: .long 0
  .globl _cgRedirLastWhere
_cgRedirLastWhere: .long 0
  .globl _cgRecSkipCount
_cgRecSkipCount: .long 0
  .globl _cgRecLastId
_cgRecLastId: .long 0
  .p2align 2
_cg_dummyRecord: .space 0x48
  .globl _cgIdSkipCount
_cgIdSkipCount: .long 0
  .globl _cgIdLastId
_cgIdLastId: .long 0
  .globl _cgIdLastCaller
_cgIdLastCaller: .long 0
  .globl _cgFxSkipCount
_cgFxSkipCount: .long 0
/* 横取りパッチの記録 */
  .globl _hookHitCount
_hookHitCount:
  .long 0x0
  .globl _fullCount
_fullCount:
  .long 0x0
  .globl _allyFullCount
_allyFullCount:
  .long 0x0
  .globl _stealSuccessCount
_stealSuccessCount:
  .long 0x0
  .globl _stealFailCount
_stealFailCount:
  .long 0x0
/* 随伴の制御の設定と記録 */
  .globl _ecMasterEnabled
_ecMasterEnabled:
  .byte 0x1
  .globl _ecAllyEscortEnabled
_ecAllyEscortEnabled:
  .byte 0x1
  .globl _ecEnemyEscortEnabled
_ecEnemyEscortEnabled:
  .byte 0x1
  .globl _ecRewriteEnabled
_ecRewriteEnabled:
  .byte 0x0
  .globl _ecStealableMask
_ecStealableMask:
  .long 0x0E0013BF
  .globl _ecAllyTypeMin
_ecAllyTypeMin:
  .long 0x1
  .globl _ecAllyTypeMax
_ecAllyTypeMax:
  .long 0xA
  .globl _ecEnemyTypeMin
_ecEnemyTypeMin:
  .long 0x15
  .globl _ecEnemyTypeMax
_ecEnemyTypeMax:
  .long 0x1E
  .globl _ecRewriteListCount
_ecRewriteListCount:
  .long 0x0
  .globl _ecRewriteList
_ecRewriteList:
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .globl _ecBaseRewriteEnabled
_ecBaseRewriteEnabled:
  .byte 0x0
  .globl _ecRoamRewriteEnabled
_ecRoamRewriteEnabled:
  .byte 0x0
  .globl _ecMemberRewriteEnabled
_ecMemberRewriteEnabled:
  .byte 0x1
  .globl _ecBaseTypeMin
_ecBaseTypeMin:
  .long 0x8E
  .globl _ecBaseTypeMax
_ecBaseTypeMax:
  .long 0xB0
  .globl _ecRoamTypeMin
_ecRoamTypeMin:
  .long 0xCA
  .globl _ecRoamTypeMax
_ecRoamTypeMax:
  .long 0xDC
  .globl _ecBaseRewriteListCount
_ecBaseRewriteListCount:
  .long 0x0
  .globl _ecBaseRewriteList
_ecBaseRewriteList:
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .globl _ecRoamRewriteListCount
_ecRoamRewriteListCount:
  .long 0x0
  .globl _ecRoamRewriteList
_ecRoamRewriteList:
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .long 0x0
  .globl _ecBaseEscortSeenCount
_ecBaseEscortSeenCount:
  .long 0x0
  .globl _ecRoamEscortSeenCount
_ecRoamEscortSeenCount:
  .long 0x0
  .globl _ecBaseRewrittenCount
_ecBaseRewrittenCount:
  .long 0x0
  .globl _ecRoamRewrittenCount
_ecRoamRewrittenCount:
  .long 0x0
  .globl _ecLastBaseEscortTypeId
_ecLastBaseEscortTypeId:
  .long 0xFFFFFFFF
  .globl _ecLastRoamEscortTypeId
_ecLastRoamEscortTypeId:
  .long 0xFFFFFFFF
  .globl _ecMemberRewrittenCount
_ecMemberRewrittenCount:
  .long 0x0
  .globl _ecLastMemberRewriteIndex
_ecLastMemberRewriteIndex:
  .long 0xFFFFFFFF
  .globl _ecLastMemberRewriteOwner
_ecLastMemberRewriteOwner:
  .long 0xFFFFFFFF
  .globl _ecBudgetFilterEnabled
_ecBudgetFilterEnabled:
  .byte 0x1
  .globl _ecBudgetSkipCount
_ecBudgetSkipCount:
  .long 0x0
  .globl _ecLastBudgetSkipIndex
_ecLastBudgetSkipIndex:
  .long 0xFFFFFFFF
  .globl _ecBudgetMemberClearedCount
_ecBudgetMemberClearedCount:
  .long 0x0
  .globl _ecRngSeed
_ecRngSeed:
  .long 0x0
  .globl _ecRngSeedInit
_ecRngSeedInit:
  .long 0x0
  .globl _ecHookFireCount
_ecHookFireCount:
  .long 0x0
  .globl _ecAllyEscortSeenCount
_ecAllyEscortSeenCount:
  .long 0x0
  .globl _ecEnemyEscortSeenCount
_ecEnemyEscortSeenCount:
  .long 0x0
  .globl _ecBlockedCount
_ecBlockedCount:
  .long 0x0
  .globl _ecRewrittenCount
_ecRewrittenCount:
  .long 0x0
  .globl _ecLastBlockedIndex
_ecLastBlockedIndex:
  .long 0xFFFFFFFF
  .globl _ecLastBlockedOwner
_ecLastBlockedOwner:
  .long 0xFFFFFFFF
  .globl _ecLastBlockedTypeId
_ecLastBlockedTypeId:
  .long 0xFFFFFFFF
  .globl _ecLastRewriteIndex
_ecLastRewriteIndex:
  .long 0xFFFFFFFF
  .globl _ecLastRewriteNewOwner
_ecLastRewriteNewOwner:
  .long 0xFFFFFFFF
  .globl _ecEnemyLinkedPercent
_ecEnemyLinkedPercent:
  .long 0x64
  .globl _ecEnemyMemberSeenCount
_ecEnemyMemberSeenCount:
  .long 0x0
  .globl _ecLinkedBlockedCount
_ecLinkedBlockedCount:
  .long 0x0
  .globl _ecLastMemberNo
_ecLastMemberNo:
  .long 0xFFFFFFFF
  .globl _ecLastLinkedBlockedIndex
_ecLastLinkedBlockedIndex:
  .long 0xFFFFFFFF
  .globl _ecProtectedMask
_ecProtectedMask:
  .long 0x33B103E1
  .globl _ecMemberOwnerPathCount
_ecMemberOwnerPathCount:
  .long 0x0
  .globl _ecLastMemberOwnerIndex
_ecLastMemberOwnerIndex:
  .long 0xFFFFFFFF
  .globl _ecLastMemberOwnerEntryOwner
_ecLastMemberOwnerEntryOwner:
  .long 0xFFFFFFFF
