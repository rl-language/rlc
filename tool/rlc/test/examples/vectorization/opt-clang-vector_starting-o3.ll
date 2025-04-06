; ModuleID = 'clang-vector-starting.ll'
source_filename = "vector_starting.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind sspstrong willreturn memory(argmem: read) uwtable
define dso_local i32 @something_32(ptr nocapture noundef readonly %0, i32 noundef %1) local_unnamed_addr #0 !dbg !10 {
    #dbg_value(ptr %0, !16, !DIExpression(), !17)
    #dbg_value(i32 %1, !18, !DIExpression(), !17)
    #dbg_value(i32 0, !18, !DIExpression(), !17)
    #dbg_value(i32 0, !19, !DIExpression(), !21)
    #dbg_value(i64 0, !19, !DIExpression(), !21)
    #dbg_value(i32 0, !18, !DIExpression(), !17)
    #dbg_value(i32 poison, !18, !DIExpression(), !17)
    #dbg_value(i64 1, !19, !DIExpression(), !21)
    #dbg_value(i32 poison, !18, !DIExpression(), !17)
    #dbg_value(i64 2, !19, !DIExpression(), !21)
    #dbg_value(i32 poison, !18, !DIExpression(), !17)
    #dbg_value(i64 3, !19, !DIExpression(), !21)
  %3 = load <4 x i32>, ptr %0, align 4, !dbg !22
  %4 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %3), !dbg !25
    #dbg_value(i32 %4, !18, !DIExpression(), !17)
    #dbg_value(i64 4, !19, !DIExpression(), !21)
  ret i32 %4, !dbg !26
}

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind sspstrong willreturn memory(argmem: read) uwtable
define dso_local i64 @something_64(ptr nocapture noundef readonly %0, i64 noundef %1) local_unnamed_addr #0 !dbg !27 {
    #dbg_value(ptr %0, !32, !DIExpression(), !33)
    #dbg_value(i64 %1, !34, !DIExpression(), !33)
    #dbg_value(i64 0, !34, !DIExpression(), !33)
    #dbg_value(i32 0, !35, !DIExpression(), !37)
    #dbg_value(i64 0, !35, !DIExpression(), !37)
    #dbg_value(i64 0, !34, !DIExpression(), !33)
    #dbg_value(i64 poison, !34, !DIExpression(), !33)
    #dbg_value(i64 1, !35, !DIExpression(), !37)
    #dbg_value(i64 poison, !34, !DIExpression(), !33)
    #dbg_value(i64 2, !35, !DIExpression(), !37)
    #dbg_value(i64 poison, !34, !DIExpression(), !33)
    #dbg_value(i64 3, !35, !DIExpression(), !37)
  %3 = load <4 x i64>, ptr %0, align 8, !dbg !38
  %4 = tail call i64 @llvm.vector.reduce.add.v4i64(<4 x i64> %3), !dbg !41
    #dbg_value(i64 %4, !34, !DIExpression(), !33)
    #dbg_value(i64 4, !35, !DIExpression(), !37)
  ret i64 %4, !dbg !42
}

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind sspstrong willreturn memory(argmem: readwrite) uwtable
define dso_local void @something_32_void(ptr nocapture noundef readonly %0, ptr nocapture noundef writeonly %1) local_unnamed_addr #1 !dbg !43 {
    #dbg_value(ptr %0, !46, !DIExpression(), !47)
    #dbg_value(ptr %1, !48, !DIExpression(), !47)
  store i32 0, ptr %1, align 4, !dbg !49
    #dbg_value(i32 0, !50, !DIExpression(), !52)
    #dbg_value(i32 0, !50, !DIExpression(), !52)
    #dbg_value(i64 0, !50, !DIExpression(), !52)
  %3 = load i32, ptr %0, align 4, !dbg !53
  store i32 %3, ptr %1, align 4, !dbg !56
    #dbg_value(i64 1, !50, !DIExpression(), !52)
  %4 = getelementptr inbounds i8, ptr %0, i64 4, !dbg !53
  %5 = load i32, ptr %4, align 4, !dbg !53
  %6 = add nsw i32 %3, %5, !dbg !56
  store i32 %6, ptr %1, align 4, !dbg !56
    #dbg_value(i64 2, !50, !DIExpression(), !52)
  %7 = getelementptr inbounds i8, ptr %0, i64 8, !dbg !53
  %8 = load i32, ptr %7, align 4, !dbg !53
  %9 = add nsw i32 %6, %8, !dbg !56
  store i32 %9, ptr %1, align 4, !dbg !56
    #dbg_value(i64 3, !50, !DIExpression(), !52)
  %10 = getelementptr inbounds i8, ptr %0, i64 12, !dbg !53
  %11 = load i32, ptr %10, align 4, !dbg !53
  %12 = add nsw i32 %9, %11, !dbg !56
  store i32 %12, ptr %1, align 4, !dbg !56
    #dbg_value(i64 4, !50, !DIExpression(), !52)
  ret void, !dbg !57
}

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind sspstrong willreturn memory(argmem: write) uwtable
define dso_local void @something_64_void(ptr nocapture noundef readnone %0, ptr nocapture noundef writeonly %1) local_unnamed_addr #2 !dbg !58 {
    #dbg_value(ptr %0, !61, !DIExpression(), !62)
    #dbg_value(ptr %1, !63, !DIExpression(), !62)
  store i64 0, ptr %1, align 8, !dbg !64
    #dbg_value(ptr poison, !63, !DIExpression(), !62)
  ret void, !dbg !65
}

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind sspstrong willreturn memory(argmem: readwrite) uwtable
define dso_local void @vector_sum(ptr nocapture noundef readonly %0, ptr nocapture noundef readonly %1, ptr nocapture noundef writeonly %2) local_unnamed_addr #1 !dbg !66 {
    #dbg_value(ptr %0, !69, !DIExpression(), !70)
    #dbg_value(ptr %1, !71, !DIExpression(), !70)
    #dbg_value(ptr %2, !72, !DIExpression(), !70)
    #dbg_value(i32 0, !73, !DIExpression(), !75)
    #dbg_value(i64 0, !73, !DIExpression(), !75)
  %4 = load i32, ptr %0, align 4, !dbg !76
  %5 = load i32, ptr %1, align 4, !dbg !79
  %6 = add nsw i32 %5, %4, !dbg !80
  store i32 %6, ptr %2, align 4, !dbg !81
    #dbg_value(i64 1, !73, !DIExpression(), !75)
  %7 = getelementptr inbounds i8, ptr %0, i64 4, !dbg !76
  %8 = load i32, ptr %7, align 4, !dbg !76
  %9 = getelementptr inbounds i8, ptr %1, i64 4, !dbg !79
  %10 = load i32, ptr %9, align 4, !dbg !79
  %11 = add nsw i32 %10, %8, !dbg !80
  %12 = getelementptr inbounds i8, ptr %2, i64 4, !dbg !82
  store i32 %11, ptr %12, align 4, !dbg !81
    #dbg_value(i64 2, !73, !DIExpression(), !75)
  %13 = getelementptr inbounds i8, ptr %0, i64 8, !dbg !76
  %14 = load i32, ptr %13, align 4, !dbg !76
  %15 = getelementptr inbounds i8, ptr %1, i64 8, !dbg !79
  %16 = load i32, ptr %15, align 4, !dbg !79
  %17 = add nsw i32 %16, %14, !dbg !80
  %18 = getelementptr inbounds i8, ptr %2, i64 8, !dbg !82
  store i32 %17, ptr %18, align 4, !dbg !81
    #dbg_value(i64 3, !73, !DIExpression(), !75)
  %19 = getelementptr inbounds i8, ptr %0, i64 12, !dbg !76
  %20 = load i32, ptr %19, align 4, !dbg !76
  %21 = getelementptr inbounds i8, ptr %1, i64 12, !dbg !79
  %22 = load i32, ptr %21, align 4, !dbg !79
  %23 = add nsw i32 %22, %20, !dbg !80
  %24 = getelementptr inbounds i8, ptr %2, i64 12, !dbg !82
  store i32 %23, ptr %24, align 4, !dbg !81
    #dbg_value(i64 4, !73, !DIExpression(), !75)
  %25 = getelementptr inbounds i8, ptr %0, i64 16, !dbg !76
  %26 = load i32, ptr %25, align 4, !dbg !76
  %27 = getelementptr inbounds i8, ptr %1, i64 16, !dbg !79
  %28 = load i32, ptr %27, align 4, !dbg !79
  %29 = add nsw i32 %28, %26, !dbg !80
  %30 = getelementptr inbounds i8, ptr %2, i64 16, !dbg !82
  store i32 %29, ptr %30, align 4, !dbg !81
    #dbg_value(i64 5, !73, !DIExpression(), !75)
  %31 = getelementptr inbounds i8, ptr %0, i64 20, !dbg !76
  %32 = load i32, ptr %31, align 4, !dbg !76
  %33 = getelementptr inbounds i8, ptr %1, i64 20, !dbg !79
  %34 = load i32, ptr %33, align 4, !dbg !79
  %35 = add nsw i32 %34, %32, !dbg !80
  %36 = getelementptr inbounds i8, ptr %2, i64 20, !dbg !82
  store i32 %35, ptr %36, align 4, !dbg !81
    #dbg_value(i64 6, !73, !DIExpression(), !75)
  %37 = getelementptr inbounds i8, ptr %0, i64 24, !dbg !76
  %38 = load i32, ptr %37, align 4, !dbg !76
  %39 = getelementptr inbounds i8, ptr %1, i64 24, !dbg !79
  %40 = load i32, ptr %39, align 4, !dbg !79
  %41 = add nsw i32 %40, %38, !dbg !80
  %42 = getelementptr inbounds i8, ptr %2, i64 24, !dbg !82
  store i32 %41, ptr %42, align 4, !dbg !81
    #dbg_value(i64 7, !73, !DIExpression(), !75)
  %43 = getelementptr inbounds i8, ptr %0, i64 28, !dbg !76
  %44 = load i32, ptr %43, align 4, !dbg !76
  %45 = getelementptr inbounds i8, ptr %1, i64 28, !dbg !79
  %46 = load i32, ptr %45, align 4, !dbg !79
  %47 = add nsw i32 %46, %44, !dbg !80
  %48 = getelementptr inbounds i8, ptr %2, i64 28, !dbg !82
  store i32 %47, ptr %48, align 4, !dbg !81
    #dbg_value(i64 8, !73, !DIExpression(), !75)
  %49 = getelementptr inbounds i8, ptr %0, i64 32, !dbg !76
  %50 = load i32, ptr %49, align 4, !dbg !76
  %51 = getelementptr inbounds i8, ptr %1, i64 32, !dbg !79
  %52 = load i32, ptr %51, align 4, !dbg !79
  %53 = add nsw i32 %52, %50, !dbg !80
  %54 = getelementptr inbounds i8, ptr %2, i64 32, !dbg !82
  store i32 %53, ptr %54, align 4, !dbg !81
    #dbg_value(i64 9, !73, !DIExpression(), !75)
  %55 = getelementptr inbounds i8, ptr %0, i64 36, !dbg !76
  %56 = load i32, ptr %55, align 4, !dbg !76
  %57 = getelementptr inbounds i8, ptr %1, i64 36, !dbg !79
  %58 = load i32, ptr %57, align 4, !dbg !79
  %59 = add nsw i32 %58, %56, !dbg !80
  %60 = getelementptr inbounds i8, ptr %2, i64 36, !dbg !82
  store i32 %59, ptr %60, align 4, !dbg !81
    #dbg_value(i64 10, !73, !DIExpression(), !75)
  ret void, !dbg !83
}

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind sspstrong willreturn memory(argmem: readwrite) uwtable
define dso_local void @vector_sum_restrict(ptr noalias nocapture noundef readonly %0, ptr noalias nocapture noundef readonly %1, ptr noalias nocapture noundef writeonly %2) local_unnamed_addr #1 !dbg !84 {
    #dbg_value(ptr %0, !88, !DIExpression(), !89)
    #dbg_value(ptr %1, !90, !DIExpression(), !89)
    #dbg_value(ptr %2, !91, !DIExpression(), !89)
    #dbg_value(i32 0, !92, !DIExpression(), !94)
    #dbg_value(i64 0, !92, !DIExpression(), !94)
    #dbg_value(i64 1, !92, !DIExpression(), !94)
    #dbg_value(i64 2, !92, !DIExpression(), !94)
    #dbg_value(i64 3, !92, !DIExpression(), !94)
  %4 = load <4 x i32>, ptr %0, align 4, !dbg !95
  %5 = load <4 x i32>, ptr %1, align 4, !dbg !98
  %6 = add nsw <4 x i32> %5, %4, !dbg !99
  store <4 x i32> %6, ptr %2, align 4, !dbg !100
    #dbg_value(i64 4, !92, !DIExpression(), !94)
  %7 = getelementptr inbounds i8, ptr %0, i64 16, !dbg !95
  %8 = getelementptr inbounds i8, ptr %1, i64 16, !dbg !98
  %9 = getelementptr inbounds i8, ptr %2, i64 16, !dbg !101
    #dbg_value(i64 5, !92, !DIExpression(), !94)
    #dbg_value(i64 6, !92, !DIExpression(), !94)
    #dbg_value(i64 7, !92, !DIExpression(), !94)
  %10 = load <4 x i32>, ptr %7, align 4, !dbg !95
  %11 = load <4 x i32>, ptr %8, align 4, !dbg !98
  %12 = add nsw <4 x i32> %11, %10, !dbg !99
  store <4 x i32> %12, ptr %9, align 4, !dbg !100
    #dbg_value(i64 8, !92, !DIExpression(), !94)
  %13 = getelementptr inbounds i8, ptr %0, i64 32, !dbg !95
  %14 = getelementptr inbounds i8, ptr %1, i64 32, !dbg !98
  %15 = getelementptr inbounds i8, ptr %2, i64 32, !dbg !101
    #dbg_value(i64 9, !92, !DIExpression(), !94)
  %16 = load <2 x i32>, ptr %13, align 4, !dbg !95
  %17 = load <2 x i32>, ptr %14, align 4, !dbg !98
  %18 = add nsw <2 x i32> %17, %16, !dbg !99
  store <2 x i32> %18, ptr %15, align 4, !dbg !100
    #dbg_value(i64 10, !92, !DIExpression(), !94)
  ret void, !dbg !102
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.vector.reduce.add.v4i64(<4 x i64>) #3

attributes #0 = { mustprogress nofree noinline norecurse nosync nounwind sspstrong willreturn memory(argmem: read) uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree noinline norecurse nosync nounwind sspstrong willreturn memory(argmem: readwrite) uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress nofree noinline norecurse nosync nounwind sspstrong willreturn memory(argmem: write) uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

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
!17 = !DILocation(line: 0, scope: !10)
!18 = !DILocalVariable(name: "b", arg: 2, scope: !10, file: !1, line: 1, type: !13)
!19 = !DILocalVariable(name: "i", scope: !20, file: !1, line: 3, type: !13)
!20 = distinct !DILexicalBlock(scope: !10, file: !1, line: 3, column: 5)
!21 = !DILocation(line: 0, scope: !20)
!22 = !DILocation(line: 4, column: 14, scope: !23)
!23 = distinct !DILexicalBlock(scope: !24, file: !1, line: 3, column: 32)
!24 = distinct !DILexicalBlock(scope: !20, file: !1, line: 3, column: 5)
!25 = !DILocation(line: 4, column: 11, scope: !23)
!26 = !DILocation(line: 7, column: 5, scope: !10)
!27 = distinct !DISubprogram(name: "something_64", scope: !1, file: !1, line: 10, type: !28, scopeLine: 10, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!28 = !DISubroutineType(types: !29)
!29 = !{!30, !31, !30}
!30 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!31 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !30, size: 64)
!32 = !DILocalVariable(name: "a", arg: 1, scope: !27, file: !1, line: 10, type: !31)
!33 = !DILocation(line: 0, scope: !27)
!34 = !DILocalVariable(name: "b", arg: 2, scope: !27, file: !1, line: 10, type: !30)
!35 = !DILocalVariable(name: "i", scope: !36, file: !1, line: 12, type: !13)
!36 = distinct !DILexicalBlock(scope: !27, file: !1, line: 12, column: 5)
!37 = !DILocation(line: 0, scope: !36)
!38 = !DILocation(line: 13, column: 14, scope: !39)
!39 = distinct !DILexicalBlock(scope: !40, file: !1, line: 12, column: 32)
!40 = distinct !DILexicalBlock(scope: !36, file: !1, line: 12, column: 5)
!41 = !DILocation(line: 13, column: 11, scope: !39)
!42 = !DILocation(line: 16, column: 5, scope: !27)
!43 = distinct !DISubprogram(name: "something_32_void", scope: !1, file: !1, line: 19, type: !44, scopeLine: 19, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!44 = !DISubroutineType(types: !45)
!45 = !{null, !14, !14}
!46 = !DILocalVariable(name: "a", arg: 1, scope: !43, file: !1, line: 19, type: !14)
!47 = !DILocation(line: 0, scope: !43)
!48 = !DILocalVariable(name: "b", arg: 2, scope: !43, file: !1, line: 19, type: !14)
!49 = !DILocation(line: 20, column: 8, scope: !43)
!50 = !DILocalVariable(name: "i", scope: !51, file: !1, line: 21, type: !13)
!51 = distinct !DILexicalBlock(scope: !43, file: !1, line: 21, column: 5)
!52 = !DILocation(line: 0, scope: !51)
!53 = !DILocation(line: 22, column: 15, scope: !54)
!54 = distinct !DILexicalBlock(scope: !55, file: !1, line: 21, column: 32)
!55 = distinct !DILexicalBlock(scope: !51, file: !1, line: 21, column: 5)
!56 = !DILocation(line: 22, column: 12, scope: !54)
!57 = !DILocation(line: 24, column: 1, scope: !43)
!58 = distinct !DISubprogram(name: "something_64_void", scope: !1, file: !1, line: 26, type: !59, scopeLine: 26, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!59 = !DISubroutineType(types: !60)
!60 = !{null, !31, !31}
!61 = !DILocalVariable(name: "a", arg: 1, scope: !58, file: !1, line: 26, type: !31)
!62 = !DILocation(line: 0, scope: !58)
!63 = !DILocalVariable(name: "b", arg: 2, scope: !58, file: !1, line: 26, type: !31)
!64 = !DILocation(line: 27, column: 8, scope: !58)
!65 = !DILocation(line: 31, column: 1, scope: !58)
!66 = distinct !DISubprogram(name: "vector_sum", scope: !1, file: !1, line: 34, type: !67, scopeLine: 34, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!67 = !DISubroutineType(types: !68)
!68 = !{null, !14, !14, !14}
!69 = !DILocalVariable(name: "a", arg: 1, scope: !66, file: !1, line: 34, type: !14)
!70 = !DILocation(line: 0, scope: !66)
!71 = !DILocalVariable(name: "b", arg: 2, scope: !66, file: !1, line: 34, type: !14)
!72 = !DILocalVariable(name: "c", arg: 3, scope: !66, file: !1, line: 34, type: !14)
!73 = !DILocalVariable(name: "i", scope: !74, file: !1, line: 36, type: !13)
!74 = distinct !DILexicalBlock(scope: !66, file: !1, line: 36, column: 5)
!75 = !DILocation(line: 0, scope: !74)
!76 = !DILocation(line: 37, column: 16, scope: !77)
!77 = distinct !DILexicalBlock(scope: !78, file: !1, line: 36, column: 32)
!78 = distinct !DILexicalBlock(scope: !74, file: !1, line: 36, column: 5)
!79 = !DILocation(line: 37, column: 23, scope: !77)
!80 = !DILocation(line: 37, column: 21, scope: !77)
!81 = !DILocation(line: 37, column: 14, scope: !77)
!82 = !DILocation(line: 37, column: 9, scope: !77)
!83 = !DILocation(line: 39, column: 1, scope: !66)
!84 = distinct !DISubprogram(name: "vector_sum_restrict", scope: !1, file: !1, line: 51, type: !85, scopeLine: 51, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!85 = !DISubroutineType(types: !86)
!86 = !{null, !87, !87, !87}
!87 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !14)
!88 = !DILocalVariable(name: "a", arg: 1, scope: !84, file: !1, line: 51, type: !87)
!89 = !DILocation(line: 0, scope: !84)
!90 = !DILocalVariable(name: "b", arg: 2, scope: !84, file: !1, line: 51, type: !87)
!91 = !DILocalVariable(name: "c", arg: 3, scope: !84, file: !1, line: 51, type: !87)
!92 = !DILocalVariable(name: "i", scope: !93, file: !1, line: 52, type: !13)
!93 = distinct !DILexicalBlock(scope: !84, file: !1, line: 52, column: 5)
!94 = !DILocation(line: 0, scope: !93)
!95 = !DILocation(line: 53, column: 16, scope: !96)
!96 = distinct !DILexicalBlock(scope: !97, file: !1, line: 52, column: 32)
!97 = distinct !DILexicalBlock(scope: !93, file: !1, line: 52, column: 5)
!98 = !DILocation(line: 53, column: 23, scope: !96)
!99 = !DILocation(line: 53, column: 21, scope: !96)
!100 = !DILocation(line: 53, column: 14, scope: !96)
!101 = !DILocation(line: 53, column: 9, scope: !96)
!102 = !DILocation(line: 55, column: 1, scope: !84)
