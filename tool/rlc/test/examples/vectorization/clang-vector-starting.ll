; ModuleID = 'vector_starting.c'
source_filename = "vector_starting.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @something_32(ptr noundef %0, i32 noundef %1) #0 !dbg !10 {
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store ptr %0, ptr %3, align 8
    #dbg_declare(ptr %3, !16, !DIExpression(), !17)
  store i32 %1, ptr %4, align 4
    #dbg_declare(ptr %4, !18, !DIExpression(), !19)
  store i32 0, ptr %4, align 4, !dbg !20
    #dbg_declare(ptr %5, !21, !DIExpression(), !23)
  store i32 0, ptr %5, align 4, !dbg !23
  br label %6, !dbg !24

6:                                                ; preds = %17, %2
  %7 = load i32, ptr %5, align 4, !dbg !25
  %8 = icmp ne i32 %7, 4, !dbg !27
  br i1 %8, label %9, label %20, !dbg !28

9:                                                ; preds = %6
  %10 = load ptr, ptr %3, align 8, !dbg !29
  %11 = load i32, ptr %5, align 4, !dbg !31
  %12 = sext i32 %11 to i64, !dbg !29
  %13 = getelementptr inbounds i32, ptr %10, i64 %12, !dbg !29
  %14 = load i32, ptr %13, align 4, !dbg !29
  %15 = load i32, ptr %4, align 4, !dbg !32
  %16 = add nsw i32 %15, %14, !dbg !32
  store i32 %16, ptr %4, align 4, !dbg !32
  br label %17, !dbg !33

17:                                               ; preds = %9
  %18 = load i32, ptr %5, align 4, !dbg !34
  %19 = add nsw i32 %18, 1, !dbg !34
  store i32 %19, ptr %5, align 4, !dbg !34
  br label %6, !dbg !35, !llvm.loop !36

20:                                               ; preds = %6
  %21 = load i32, ptr %4, align 4, !dbg !39
  ret i32 %21, !dbg !40
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i64 @something_64(ptr noundef %0, i64 noundef %1) #0 !dbg !41 {
  %3 = alloca ptr, align 8
  %4 = alloca i64, align 8
  %5 = alloca i32, align 4
  store ptr %0, ptr %3, align 8
    #dbg_declare(ptr %3, !46, !DIExpression(), !47)
  store i64 %1, ptr %4, align 8
    #dbg_declare(ptr %4, !48, !DIExpression(), !49)
  store i64 0, ptr %4, align 8, !dbg !50
    #dbg_declare(ptr %5, !51, !DIExpression(), !53)
  store i32 0, ptr %5, align 4, !dbg !53
  br label %6, !dbg !54

6:                                                ; preds = %17, %2
  %7 = load i32, ptr %5, align 4, !dbg !55
  %8 = icmp ne i32 %7, 4, !dbg !57
  br i1 %8, label %9, label %20, !dbg !58

9:                                                ; preds = %6
  %10 = load ptr, ptr %3, align 8, !dbg !59
  %11 = load i32, ptr %5, align 4, !dbg !61
  %12 = sext i32 %11 to i64, !dbg !59
  %13 = getelementptr inbounds i64, ptr %10, i64 %12, !dbg !59
  %14 = load i64, ptr %13, align 8, !dbg !59
  %15 = load i64, ptr %4, align 8, !dbg !62
  %16 = add nsw i64 %15, %14, !dbg !62
  store i64 %16, ptr %4, align 8, !dbg !62
  br label %17, !dbg !63

17:                                               ; preds = %9
  %18 = load i32, ptr %5, align 4, !dbg !64
  %19 = add nsw i32 %18, 1, !dbg !64
  store i32 %19, ptr %5, align 4, !dbg !64
  br label %6, !dbg !65, !llvm.loop !66

20:                                               ; preds = %6
  %21 = load i64, ptr %4, align 8, !dbg !68
  ret i64 %21, !dbg !69
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @something_32_void(ptr noundef %0, ptr noundef %1) #0 !dbg !70 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  store ptr %0, ptr %3, align 8
    #dbg_declare(ptr %3, !73, !DIExpression(), !74)
  store ptr %1, ptr %4, align 8
    #dbg_declare(ptr %4, !75, !DIExpression(), !76)
  %6 = load ptr, ptr %4, align 8, !dbg !77
  store i32 0, ptr %6, align 4, !dbg !78
    #dbg_declare(ptr %5, !79, !DIExpression(), !81)
  store i32 0, ptr %5, align 4, !dbg !81
  br label %7, !dbg !82

7:                                                ; preds = %19, %2
  %8 = load i32, ptr %5, align 4, !dbg !83
  %9 = icmp ne i32 %8, 4, !dbg !85
  br i1 %9, label %10, label %22, !dbg !86

10:                                               ; preds = %7
  %11 = load ptr, ptr %3, align 8, !dbg !87
  %12 = load i32, ptr %5, align 4, !dbg !89
  %13 = sext i32 %12 to i64, !dbg !87
  %14 = getelementptr inbounds i32, ptr %11, i64 %13, !dbg !87
  %15 = load i32, ptr %14, align 4, !dbg !87
  %16 = load ptr, ptr %4, align 8, !dbg !90
  %17 = load i32, ptr %16, align 4, !dbg !91
  %18 = add nsw i32 %17, %15, !dbg !91
  store i32 %18, ptr %16, align 4, !dbg !91
  br label %19, !dbg !92

19:                                               ; preds = %10
  %20 = load i32, ptr %5, align 4, !dbg !93
  %21 = add nsw i32 %20, 1, !dbg !93
  store i32 %21, ptr %5, align 4, !dbg !93
  br label %7, !dbg !94, !llvm.loop !95

22:                                               ; preds = %7
  ret void, !dbg !97
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @something_64_void(ptr noundef %0, ptr noundef %1) #0 !dbg !98 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  store ptr %0, ptr %3, align 8
    #dbg_declare(ptr %3, !101, !DIExpression(), !102)
  store ptr %1, ptr %4, align 8
    #dbg_declare(ptr %4, !103, !DIExpression(), !104)
  %6 = load ptr, ptr %4, align 8, !dbg !105
  store i64 0, ptr %6, align 8, !dbg !106
    #dbg_declare(ptr %5, !107, !DIExpression(), !109)
  store i32 0, ptr %5, align 4, !dbg !109
  br label %7, !dbg !110

7:                                                ; preds = %18, %2
  %8 = load i32, ptr %5, align 4, !dbg !111
  %9 = icmp ne i32 %8, 4, !dbg !113
  br i1 %9, label %10, label %21, !dbg !114

10:                                               ; preds = %7
  %11 = load ptr, ptr %3, align 8, !dbg !115
  %12 = load i32, ptr %5, align 4, !dbg !117
  %13 = sext i32 %12 to i64, !dbg !115
  %14 = getelementptr inbounds i64, ptr %11, i64 %13, !dbg !115
  %15 = load i64, ptr %14, align 8, !dbg !115
  %16 = load ptr, ptr %4, align 8, !dbg !118
  %17 = getelementptr inbounds i64, ptr %16, i64 %15, !dbg !118
  store ptr %17, ptr %4, align 8, !dbg !118
  br label %18, !dbg !119

18:                                               ; preds = %10
  %19 = load i32, ptr %5, align 4, !dbg !120
  %20 = add nsw i32 %19, 1, !dbg !120
  store i32 %20, ptr %5, align 4, !dbg !120
  br label %7, !dbg !121, !llvm.loop !122

21:                                               ; preds = %7
  ret void, !dbg !124
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @vector_sum(ptr noundef %0, ptr noundef %1, ptr noundef %2) #0 !dbg !125 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  store ptr %0, ptr %4, align 8
    #dbg_declare(ptr %4, !128, !DIExpression(), !129)
  store ptr %1, ptr %5, align 8
    #dbg_declare(ptr %5, !130, !DIExpression(), !131)
  store ptr %2, ptr %6, align 8
    #dbg_declare(ptr %6, !132, !DIExpression(), !133)
    #dbg_declare(ptr %7, !134, !DIExpression(), !136)
  store i32 0, ptr %7, align 4, !dbg !136
  br label %8, !dbg !137

8:                                                ; preds = %27, %3
  %9 = load i32, ptr %7, align 4, !dbg !138
  %10 = icmp slt i32 %9, 10, !dbg !140
  br i1 %10, label %11, label %30, !dbg !141

11:                                               ; preds = %8
  %12 = load ptr, ptr %4, align 8, !dbg !142
  %13 = load i32, ptr %7, align 4, !dbg !144
  %14 = sext i32 %13 to i64, !dbg !142
  %15 = getelementptr inbounds i32, ptr %12, i64 %14, !dbg !142
  %16 = load i32, ptr %15, align 4, !dbg !142
  %17 = load ptr, ptr %5, align 8, !dbg !145
  %18 = load i32, ptr %7, align 4, !dbg !146
  %19 = sext i32 %18 to i64, !dbg !145
  %20 = getelementptr inbounds i32, ptr %17, i64 %19, !dbg !145
  %21 = load i32, ptr %20, align 4, !dbg !145
  %22 = add nsw i32 %16, %21, !dbg !147
  %23 = load ptr, ptr %6, align 8, !dbg !148
  %24 = load i32, ptr %7, align 4, !dbg !149
  %25 = sext i32 %24 to i64, !dbg !148
  %26 = getelementptr inbounds i32, ptr %23, i64 %25, !dbg !148
  store i32 %22, ptr %26, align 4, !dbg !150
  br label %27, !dbg !151

27:                                               ; preds = %11
  %28 = load i32, ptr %7, align 4, !dbg !152
  %29 = add nsw i32 %28, 1, !dbg !152
  store i32 %29, ptr %7, align 4, !dbg !152
  br label %8, !dbg !153, !llvm.loop !154

30:                                               ; preds = %8
  ret void, !dbg !156
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @vector_sum_restrict(ptr noalias noundef %0, ptr noalias noundef %1, ptr noalias noundef %2) #0 !dbg !157 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  store ptr %0, ptr %4, align 8
    #dbg_declare(ptr %4, !161, !DIExpression(), !162)
  store ptr %1, ptr %5, align 8
    #dbg_declare(ptr %5, !163, !DIExpression(), !164)
  store ptr %2, ptr %6, align 8
    #dbg_declare(ptr %6, !165, !DIExpression(), !166)
    #dbg_declare(ptr %7, !167, !DIExpression(), !169)
  store i32 0, ptr %7, align 4, !dbg !169
  br label %8, !dbg !170

8:                                                ; preds = %27, %3
  %9 = load i32, ptr %7, align 4, !dbg !171
  %10 = icmp slt i32 %9, 10, !dbg !173
  br i1 %10, label %11, label %30, !dbg !174

11:                                               ; preds = %8
  %12 = load ptr, ptr %4, align 8, !dbg !175
  %13 = load i32, ptr %7, align 4, !dbg !177
  %14 = sext i32 %13 to i64, !dbg !175
  %15 = getelementptr inbounds i32, ptr %12, i64 %14, !dbg !175
  %16 = load i32, ptr %15, align 4, !dbg !175
  %17 = load ptr, ptr %5, align 8, !dbg !178
  %18 = load i32, ptr %7, align 4, !dbg !179
  %19 = sext i32 %18 to i64, !dbg !178
  %20 = getelementptr inbounds i32, ptr %17, i64 %19, !dbg !178
  %21 = load i32, ptr %20, align 4, !dbg !178
  %22 = add nsw i32 %16, %21, !dbg !180
  %23 = load ptr, ptr %6, align 8, !dbg !181
  %24 = load i32, ptr %7, align 4, !dbg !182
  %25 = sext i32 %24 to i64, !dbg !181
  %26 = getelementptr inbounds i32, ptr %23, i64 %25, !dbg !181
  store i32 %22, ptr %26, align 4, !dbg !183
  br label %27, !dbg !184

27:                                               ; preds = %11
  %28 = load i32, ptr %7, align 4, !dbg !185
  %29 = add nsw i32 %28, 1, !dbg !185
  store i32 %29, ptr %7, align 4, !dbg !185
  br label %8, !dbg !186, !llvm.loop !187

30:                                               ; preds = %8
  ret void, !dbg !189
}

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 19.1.7", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "vector_starting.c", directory: "/run/media/thanos/RLC/rlc-infrastructure/rlc/tool/rlc/test/examples/vectorization", checksumkind: CSK_MD5, checksum: "c4b3430321e0ee3527318e077a372e7c")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"clang version 19.1.7"}
!10 = distinct !DISubprogram(name: "something_32", scope: !1, file: !1, line: 1, type: !11, scopeLine: 1, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!11 = !DISubroutineType(types: !12)
!12 = !{!13, !14, !13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!15 = !{}
!16 = !DILocalVariable(name: "a", arg: 1, scope: !10, file: !1, line: 1, type: !14)
!17 = !DILocation(line: 1, column: 22, scope: !10)
!18 = !DILocalVariable(name: "b", arg: 2, scope: !10, file: !1, line: 1, type: !13)
!19 = !DILocation(line: 1, column: 32, scope: !10)
!20 = !DILocation(line: 2, column: 7, scope: !10)
!21 = !DILocalVariable(name: "i", scope: !22, file: !1, line: 3, type: !13)
!22 = distinct !DILexicalBlock(scope: !10, file: !1, line: 3, column: 5)
!23 = !DILocation(line: 3, column: 13, scope: !22)
!24 = !DILocation(line: 3, column: 9, scope: !22)
!25 = !DILocation(line: 3, column: 20, scope: !26)
!26 = distinct !DILexicalBlock(scope: !22, file: !1, line: 3, column: 5)
!27 = !DILocation(line: 3, column: 22, scope: !26)
!28 = !DILocation(line: 3, column: 5, scope: !22)
!29 = !DILocation(line: 4, column: 14, scope: !30)
!30 = distinct !DILexicalBlock(scope: !26, file: !1, line: 3, column: 32)
!31 = !DILocation(line: 4, column: 16, scope: !30)
!32 = !DILocation(line: 4, column: 11, scope: !30)
!33 = !DILocation(line: 5, column: 5, scope: !30)
!34 = !DILocation(line: 3, column: 29, scope: !26)
!35 = !DILocation(line: 3, column: 5, scope: !26)
!36 = distinct !{!36, !28, !37, !38}
!37 = !DILocation(line: 5, column: 5, scope: !22)
!38 = !{!"llvm.loop.mustprogress"}
!39 = !DILocation(line: 7, column: 12, scope: !10)
!40 = !DILocation(line: 7, column: 5, scope: !10)
!41 = distinct !DISubprogram(name: "something_64", scope: !1, file: !1, line: 10, type: !42, scopeLine: 10, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!42 = !DISubroutineType(types: !43)
!43 = !{!44, !45, !44}
!44 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!45 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !44, size: 64)
!46 = !DILocalVariable(name: "a", arg: 1, scope: !41, file: !1, line: 10, type: !45)
!47 = !DILocation(line: 10, column: 24, scope: !41)
!48 = !DILocalVariable(name: "b", arg: 2, scope: !41, file: !1, line: 10, type: !44)
!49 = !DILocation(line: 10, column: 35, scope: !41)
!50 = !DILocation(line: 11, column: 7, scope: !41)
!51 = !DILocalVariable(name: "i", scope: !52, file: !1, line: 12, type: !13)
!52 = distinct !DILexicalBlock(scope: !41, file: !1, line: 12, column: 5)
!53 = !DILocation(line: 12, column: 13, scope: !52)
!54 = !DILocation(line: 12, column: 9, scope: !52)
!55 = !DILocation(line: 12, column: 20, scope: !56)
!56 = distinct !DILexicalBlock(scope: !52, file: !1, line: 12, column: 5)
!57 = !DILocation(line: 12, column: 22, scope: !56)
!58 = !DILocation(line: 12, column: 5, scope: !52)
!59 = !DILocation(line: 13, column: 14, scope: !60)
!60 = distinct !DILexicalBlock(scope: !56, file: !1, line: 12, column: 32)
!61 = !DILocation(line: 13, column: 16, scope: !60)
!62 = !DILocation(line: 13, column: 11, scope: !60)
!63 = !DILocation(line: 14, column: 5, scope: !60)
!64 = !DILocation(line: 12, column: 29, scope: !56)
!65 = !DILocation(line: 12, column: 5, scope: !56)
!66 = distinct !{!66, !58, !67, !38}
!67 = !DILocation(line: 14, column: 5, scope: !52)
!68 = !DILocation(line: 16, column: 12, scope: !41)
!69 = !DILocation(line: 16, column: 5, scope: !41)
!70 = distinct !DISubprogram(name: "something_32_void", scope: !1, file: !1, line: 19, type: !71, scopeLine: 19, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!71 = !DISubroutineType(types: !72)
!72 = !{null, !14, !14}
!73 = !DILocalVariable(name: "a", arg: 1, scope: !70, file: !1, line: 19, type: !14)
!74 = !DILocation(line: 19, column: 28, scope: !70)
!75 = !DILocalVariable(name: "b", arg: 2, scope: !70, file: !1, line: 19, type: !14)
!76 = !DILocation(line: 19, column: 39, scope: !70)
!77 = !DILocation(line: 20, column: 6, scope: !70)
!78 = !DILocation(line: 20, column: 8, scope: !70)
!79 = !DILocalVariable(name: "i", scope: !80, file: !1, line: 21, type: !13)
!80 = distinct !DILexicalBlock(scope: !70, file: !1, line: 21, column: 5)
!81 = !DILocation(line: 21, column: 13, scope: !80)
!82 = !DILocation(line: 21, column: 9, scope: !80)
!83 = !DILocation(line: 21, column: 20, scope: !84)
!84 = distinct !DILexicalBlock(scope: !80, file: !1, line: 21, column: 5)
!85 = !DILocation(line: 21, column: 22, scope: !84)
!86 = !DILocation(line: 21, column: 5, scope: !80)
!87 = !DILocation(line: 22, column: 15, scope: !88)
!88 = distinct !DILexicalBlock(scope: !84, file: !1, line: 21, column: 32)
!89 = !DILocation(line: 22, column: 17, scope: !88)
!90 = !DILocation(line: 22, column: 10, scope: !88)
!91 = !DILocation(line: 22, column: 12, scope: !88)
!92 = !DILocation(line: 23, column: 5, scope: !88)
!93 = !DILocation(line: 21, column: 29, scope: !84)
!94 = !DILocation(line: 21, column: 5, scope: !84)
!95 = distinct !{!95, !86, !96, !38}
!96 = !DILocation(line: 23, column: 5, scope: !80)
!97 = !DILocation(line: 24, column: 1, scope: !70)
!98 = distinct !DISubprogram(name: "something_64_void", scope: !1, file: !1, line: 26, type: !99, scopeLine: 26, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!99 = !DISubroutineType(types: !100)
!100 = !{null, !45, !45}
!101 = !DILocalVariable(name: "a", arg: 1, scope: !98, file: !1, line: 26, type: !45)
!102 = !DILocation(line: 26, column: 29, scope: !98)
!103 = !DILocalVariable(name: "b", arg: 2, scope: !98, file: !1, line: 26, type: !45)
!104 = !DILocation(line: 26, column: 41, scope: !98)
!105 = !DILocation(line: 27, column: 6, scope: !98)
!106 = !DILocation(line: 27, column: 8, scope: !98)
!107 = !DILocalVariable(name: "i", scope: !108, file: !1, line: 28, type: !13)
!108 = distinct !DILexicalBlock(scope: !98, file: !1, line: 28, column: 5)
!109 = !DILocation(line: 28, column: 13, scope: !108)
!110 = !DILocation(line: 28, column: 9, scope: !108)
!111 = !DILocation(line: 28, column: 20, scope: !112)
!112 = distinct !DILexicalBlock(scope: !108, file: !1, line: 28, column: 5)
!113 = !DILocation(line: 28, column: 22, scope: !112)
!114 = !DILocation(line: 28, column: 5, scope: !108)
!115 = !DILocation(line: 29, column: 14, scope: !116)
!116 = distinct !DILexicalBlock(scope: !112, file: !1, line: 28, column: 32)
!117 = !DILocation(line: 29, column: 16, scope: !116)
!118 = !DILocation(line: 29, column: 11, scope: !116)
!119 = !DILocation(line: 30, column: 5, scope: !116)
!120 = !DILocation(line: 28, column: 29, scope: !112)
!121 = !DILocation(line: 28, column: 5, scope: !112)
!122 = distinct !{!122, !114, !123, !38}
!123 = !DILocation(line: 30, column: 5, scope: !108)
!124 = !DILocation(line: 31, column: 1, scope: !98)
!125 = distinct !DISubprogram(name: "vector_sum", scope: !1, file: !1, line: 34, type: !126, scopeLine: 34, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!126 = !DISubroutineType(types: !127)
!127 = !{null, !14, !14, !14}
!128 = !DILocalVariable(name: "a", arg: 1, scope: !125, file: !1, line: 34, type: !14)
!129 = !DILocation(line: 34, column: 21, scope: !125)
!130 = !DILocalVariable(name: "b", arg: 2, scope: !125, file: !1, line: 34, type: !14)
!131 = !DILocation(line: 34, column: 32, scope: !125)
!132 = !DILocalVariable(name: "c", arg: 3, scope: !125, file: !1, line: 34, type: !14)
!133 = !DILocation(line: 34, column: 43, scope: !125)
!134 = !DILocalVariable(name: "i", scope: !135, file: !1, line: 36, type: !13)
!135 = distinct !DILexicalBlock(scope: !125, file: !1, line: 36, column: 5)
!136 = !DILocation(line: 36, column: 13, scope: !135)
!137 = !DILocation(line: 36, column: 9, scope: !135)
!138 = !DILocation(line: 36, column: 20, scope: !139)
!139 = distinct !DILexicalBlock(scope: !135, file: !1, line: 36, column: 5)
!140 = !DILocation(line: 36, column: 22, scope: !139)
!141 = !DILocation(line: 36, column: 5, scope: !135)
!142 = !DILocation(line: 37, column: 16, scope: !143)
!143 = distinct !DILexicalBlock(scope: !139, file: !1, line: 36, column: 32)
!144 = !DILocation(line: 37, column: 18, scope: !143)
!145 = !DILocation(line: 37, column: 23, scope: !143)
!146 = !DILocation(line: 37, column: 25, scope: !143)
!147 = !DILocation(line: 37, column: 21, scope: !143)
!148 = !DILocation(line: 37, column: 9, scope: !143)
!149 = !DILocation(line: 37, column: 11, scope: !143)
!150 = !DILocation(line: 37, column: 14, scope: !143)
!151 = !DILocation(line: 38, column: 5, scope: !143)
!152 = !DILocation(line: 36, column: 29, scope: !139)
!153 = !DILocation(line: 36, column: 5, scope: !139)
!154 = distinct !{!154, !141, !155, !38}
!155 = !DILocation(line: 38, column: 5, scope: !135)
!156 = !DILocation(line: 39, column: 1, scope: !125)
!157 = distinct !DISubprogram(name: "vector_sum_restrict", scope: !1, file: !1, line: 51, type: !158, scopeLine: 51, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!158 = !DISubroutineType(types: !159)
!159 = !{null, !160, !160, !160}
!160 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !14)
!161 = !DILocalVariable(name: "a", arg: 1, scope: !157, file: !1, line: 51, type: !160)
!162 = !DILocation(line: 51, column: 40, scope: !157)
!163 = !DILocalVariable(name: "b", arg: 2, scope: !157, file: !1, line: 51, type: !160)
!164 = !DILocation(line: 51, column: 57, scope: !157)
!165 = !DILocalVariable(name: "c", arg: 3, scope: !157, file: !1, line: 51, type: !160)
!166 = !DILocation(line: 51, column: 74, scope: !157)
!167 = !DILocalVariable(name: "i", scope: !168, file: !1, line: 52, type: !13)
!168 = distinct !DILexicalBlock(scope: !157, file: !1, line: 52, column: 5)
!169 = !DILocation(line: 52, column: 13, scope: !168)
!170 = !DILocation(line: 52, column: 9, scope: !168)
!171 = !DILocation(line: 52, column: 20, scope: !172)
!172 = distinct !DILexicalBlock(scope: !168, file: !1, line: 52, column: 5)
!173 = !DILocation(line: 52, column: 22, scope: !172)
!174 = !DILocation(line: 52, column: 5, scope: !168)
!175 = !DILocation(line: 53, column: 16, scope: !176)
!176 = distinct !DILexicalBlock(scope: !172, file: !1, line: 52, column: 32)
!177 = !DILocation(line: 53, column: 18, scope: !176)
!178 = !DILocation(line: 53, column: 23, scope: !176)
!179 = !DILocation(line: 53, column: 25, scope: !176)
!180 = !DILocation(line: 53, column: 21, scope: !176)
!181 = !DILocation(line: 53, column: 9, scope: !176)
!182 = !DILocation(line: 53, column: 11, scope: !176)
!183 = !DILocation(line: 53, column: 14, scope: !176)
!184 = !DILocation(line: 54, column: 5, scope: !176)
!185 = !DILocation(line: 52, column: 29, scope: !172)
!186 = !DILocation(line: 52, column: 5, scope: !172)
!187 = distinct !{!187, !174, !188, !38}
!188 = !DILocation(line: 54, column: 5, scope: !168)
!189 = !DILocation(line: 55, column: 1, scope: !157)
