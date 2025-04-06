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

7:                                                ; preds = %19, %2
  %8 = load i32, ptr %5, align 4, !dbg !111
  %9 = icmp ne i32 %8, 4, !dbg !113
  br i1 %9, label %10, label %22, !dbg !114

10:                                               ; preds = %7
  %11 = load ptr, ptr %3, align 8, !dbg !115
  %12 = load i32, ptr %5, align 4, !dbg !117
  %13 = sext i32 %12 to i64, !dbg !115
  %14 = getelementptr inbounds i64, ptr %11, i64 %13, !dbg !115
  %15 = load i64, ptr %14, align 8, !dbg !115
  %16 = load ptr, ptr %4, align 8, !dbg !118
  %17 = load i64, ptr %16, align 8, !dbg !119
  %18 = add nsw i64 %17, %15, !dbg !119
  store i64 %18, ptr %16, align 8, !dbg !119
  br label %19, !dbg !120

19:                                               ; preds = %10
  %20 = load i32, ptr %5, align 4, !dbg !121
  %21 = add nsw i32 %20, 1, !dbg !121
  store i32 %21, ptr %5, align 4, !dbg !121
  br label %7, !dbg !122, !llvm.loop !123

22:                                               ; preds = %7
  ret void, !dbg !125
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @vector_sum(ptr noundef %0, ptr noundef %1, ptr noundef %2) #0 !dbg !126 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  store ptr %0, ptr %4, align 8
    #dbg_declare(ptr %4, !129, !DIExpression(), !130)
  store ptr %1, ptr %5, align 8
    #dbg_declare(ptr %5, !131, !DIExpression(), !132)
  store ptr %2, ptr %6, align 8
    #dbg_declare(ptr %6, !133, !DIExpression(), !134)
    #dbg_declare(ptr %7, !135, !DIExpression(), !137)
  store i32 0, ptr %7, align 4, !dbg !137
  br label %8, !dbg !138

8:                                                ; preds = %27, %3
  %9 = load i32, ptr %7, align 4, !dbg !139
  %10 = icmp slt i32 %9, 10, !dbg !141
  br i1 %10, label %11, label %30, !dbg !142

11:                                               ; preds = %8
  %12 = load ptr, ptr %4, align 8, !dbg !143
  %13 = load i32, ptr %7, align 4, !dbg !145
  %14 = sext i32 %13 to i64, !dbg !143
  %15 = getelementptr inbounds i32, ptr %12, i64 %14, !dbg !143
  %16 = load i32, ptr %15, align 4, !dbg !143
  %17 = load ptr, ptr %5, align 8, !dbg !146
  %18 = load i32, ptr %7, align 4, !dbg !147
  %19 = sext i32 %18 to i64, !dbg !146
  %20 = getelementptr inbounds i32, ptr %17, i64 %19, !dbg !146
  %21 = load i32, ptr %20, align 4, !dbg !146
  %22 = add nsw i32 %16, %21, !dbg !148
  %23 = load ptr, ptr %6, align 8, !dbg !149
  %24 = load i32, ptr %7, align 4, !dbg !150
  %25 = sext i32 %24 to i64, !dbg !149
  %26 = getelementptr inbounds i32, ptr %23, i64 %25, !dbg !149
  store i32 %22, ptr %26, align 4, !dbg !151
  br label %27, !dbg !152

27:                                               ; preds = %11
  %28 = load i32, ptr %7, align 4, !dbg !153
  %29 = add nsw i32 %28, 1, !dbg !153
  store i32 %29, ptr %7, align 4, !dbg !153
  br label %8, !dbg !154, !llvm.loop !155

30:                                               ; preds = %8
  ret void, !dbg !157
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @vector_sum_restrict(ptr noalias noundef %0, ptr noalias noundef %1, ptr noalias noundef %2) #0 !dbg !158 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  store ptr %0, ptr %4, align 8
    #dbg_declare(ptr %4, !162, !DIExpression(), !163)
  store ptr %1, ptr %5, align 8
    #dbg_declare(ptr %5, !164, !DIExpression(), !165)
  store ptr %2, ptr %6, align 8
    #dbg_declare(ptr %6, !166, !DIExpression(), !167)
    #dbg_declare(ptr %7, !168, !DIExpression(), !170)
  store i32 0, ptr %7, align 4, !dbg !170
  br label %8, !dbg !171

8:                                                ; preds = %27, %3
  %9 = load i32, ptr %7, align 4, !dbg !172
  %10 = icmp slt i32 %9, 10, !dbg !174
  br i1 %10, label %11, label %30, !dbg !175

11:                                               ; preds = %8
  %12 = load ptr, ptr %4, align 8, !dbg !176
  %13 = load i32, ptr %7, align 4, !dbg !178
  %14 = sext i32 %13 to i64, !dbg !176
  %15 = getelementptr inbounds i32, ptr %12, i64 %14, !dbg !176
  %16 = load i32, ptr %15, align 4, !dbg !176
  %17 = load ptr, ptr %5, align 8, !dbg !179
  %18 = load i32, ptr %7, align 4, !dbg !180
  %19 = sext i32 %18 to i64, !dbg !179
  %20 = getelementptr inbounds i32, ptr %17, i64 %19, !dbg !179
  %21 = load i32, ptr %20, align 4, !dbg !179
  %22 = add nsw i32 %16, %21, !dbg !181
  %23 = load ptr, ptr %6, align 8, !dbg !182
  %24 = load i32, ptr %7, align 4, !dbg !183
  %25 = sext i32 %24 to i64, !dbg !182
  %26 = getelementptr inbounds i32, ptr %23, i64 %25, !dbg !182
  store i32 %22, ptr %26, align 4, !dbg !184
  br label %27, !dbg !185

27:                                               ; preds = %11
  %28 = load i32, ptr %7, align 4, !dbg !186
  %29 = add nsw i32 %28, 1, !dbg !186
  store i32 %29, ptr %7, align 4, !dbg !186
  br label %8, !dbg !187, !llvm.loop !188

30:                                               ; preds = %8
  ret void, !dbg !190
}

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 19.1.7", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "vector_starting.c", directory: "/run/media/thanos/RLC/rlc-infrastructure/rlc/tool/rlc/test/examples/vectorization", checksumkind: CSK_MD5, checksum: "62f4516b4b0fff606a350638300d1106")
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
!115 = !DILocation(line: 29, column: 15, scope: !116)
!116 = distinct !DILexicalBlock(scope: !112, file: !1, line: 28, column: 32)
!117 = !DILocation(line: 29, column: 17, scope: !116)
!118 = !DILocation(line: 29, column: 10, scope: !116)
!119 = !DILocation(line: 29, column: 12, scope: !116)
!120 = !DILocation(line: 30, column: 5, scope: !116)
!121 = !DILocation(line: 28, column: 29, scope: !112)
!122 = !DILocation(line: 28, column: 5, scope: !112)
!123 = distinct !{!123, !114, !124, !38}
!124 = !DILocation(line: 30, column: 5, scope: !108)
!125 = !DILocation(line: 31, column: 1, scope: !98)
!126 = distinct !DISubprogram(name: "vector_sum", scope: !1, file: !1, line: 34, type: !127, scopeLine: 34, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!127 = !DISubroutineType(types: !128)
!128 = !{null, !14, !14, !14}
!129 = !DILocalVariable(name: "a", arg: 1, scope: !126, file: !1, line: 34, type: !14)
!130 = !DILocation(line: 34, column: 21, scope: !126)
!131 = !DILocalVariable(name: "b", arg: 2, scope: !126, file: !1, line: 34, type: !14)
!132 = !DILocation(line: 34, column: 32, scope: !126)
!133 = !DILocalVariable(name: "c", arg: 3, scope: !126, file: !1, line: 34, type: !14)
!134 = !DILocation(line: 34, column: 43, scope: !126)
!135 = !DILocalVariable(name: "i", scope: !136, file: !1, line: 36, type: !13)
!136 = distinct !DILexicalBlock(scope: !126, file: !1, line: 36, column: 5)
!137 = !DILocation(line: 36, column: 13, scope: !136)
!138 = !DILocation(line: 36, column: 9, scope: !136)
!139 = !DILocation(line: 36, column: 20, scope: !140)
!140 = distinct !DILexicalBlock(scope: !136, file: !1, line: 36, column: 5)
!141 = !DILocation(line: 36, column: 22, scope: !140)
!142 = !DILocation(line: 36, column: 5, scope: !136)
!143 = !DILocation(line: 37, column: 16, scope: !144)
!144 = distinct !DILexicalBlock(scope: !140, file: !1, line: 36, column: 32)
!145 = !DILocation(line: 37, column: 18, scope: !144)
!146 = !DILocation(line: 37, column: 23, scope: !144)
!147 = !DILocation(line: 37, column: 25, scope: !144)
!148 = !DILocation(line: 37, column: 21, scope: !144)
!149 = !DILocation(line: 37, column: 9, scope: !144)
!150 = !DILocation(line: 37, column: 11, scope: !144)
!151 = !DILocation(line: 37, column: 14, scope: !144)
!152 = !DILocation(line: 38, column: 5, scope: !144)
!153 = !DILocation(line: 36, column: 29, scope: !140)
!154 = !DILocation(line: 36, column: 5, scope: !140)
!155 = distinct !{!155, !142, !156, !38}
!156 = !DILocation(line: 38, column: 5, scope: !136)
!157 = !DILocation(line: 39, column: 1, scope: !126)
!158 = distinct !DISubprogram(name: "vector_sum_restrict", scope: !1, file: !1, line: 51, type: !159, scopeLine: 51, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!159 = !DISubroutineType(types: !160)
!160 = !{null, !161, !161, !161}
!161 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !14)
!162 = !DILocalVariable(name: "a", arg: 1, scope: !158, file: !1, line: 51, type: !161)
!163 = !DILocation(line: 51, column: 40, scope: !158)
!164 = !DILocalVariable(name: "b", arg: 2, scope: !158, file: !1, line: 51, type: !161)
!165 = !DILocation(line: 51, column: 57, scope: !158)
!166 = !DILocalVariable(name: "c", arg: 3, scope: !158, file: !1, line: 51, type: !161)
!167 = !DILocation(line: 51, column: 74, scope: !158)
!168 = !DILocalVariable(name: "i", scope: !169, file: !1, line: 52, type: !13)
!169 = distinct !DILexicalBlock(scope: !158, file: !1, line: 52, column: 5)
!170 = !DILocation(line: 52, column: 13, scope: !169)
!171 = !DILocation(line: 52, column: 9, scope: !169)
!172 = !DILocation(line: 52, column: 20, scope: !173)
!173 = distinct !DILexicalBlock(scope: !169, file: !1, line: 52, column: 5)
!174 = !DILocation(line: 52, column: 22, scope: !173)
!175 = !DILocation(line: 52, column: 5, scope: !169)
!176 = !DILocation(line: 53, column: 16, scope: !177)
!177 = distinct !DILexicalBlock(scope: !173, file: !1, line: 52, column: 32)
!178 = !DILocation(line: 53, column: 18, scope: !177)
!179 = !DILocation(line: 53, column: 23, scope: !177)
!180 = !DILocation(line: 53, column: 25, scope: !177)
!181 = !DILocation(line: 53, column: 21, scope: !177)
!182 = !DILocation(line: 53, column: 9, scope: !177)
!183 = !DILocation(line: 53, column: 11, scope: !177)
!184 = !DILocation(line: 53, column: 14, scope: !177)
!185 = !DILocation(line: 54, column: 5, scope: !177)
!186 = !DILocation(line: 52, column: 29, scope: !173)
!187 = !DILocation(line: 52, column: 5, scope: !173)
!188 = distinct !{!188, !175, !189, !38}
!189 = !DILocation(line: 54, column: 5, scope: !169)
!190 = !DILocation(line: 55, column: 1, scope: !158)
