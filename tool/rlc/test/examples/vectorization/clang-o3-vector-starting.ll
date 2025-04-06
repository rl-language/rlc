; ModuleID = 'vector_starting.c'
source_filename = "vector_starting.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: read) uwtable
define dso_local i32 @something_32(ptr nocapture noundef readonly %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = load <4 x i32>, ptr %0, align 4, !tbaa !5
  %4 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %3)
  ret i32 %4
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: read) uwtable
define dso_local i64 @something_64(ptr nocapture noundef readonly %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = load <4 x i64>, ptr %0, align 8, !tbaa !9
  %4 = tail call i64 @llvm.vector.reduce.add.v4i64(<4 x i64> %3)
  ret i64 %4
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: readwrite) uwtable
define dso_local void @something_32_void(ptr nocapture noundef readonly %0, ptr nocapture noundef writeonly %1) local_unnamed_addr #1 {
  store i32 0, ptr %1, align 4, !tbaa !5
  %3 = load i32, ptr %0, align 4, !tbaa !5
  store i32 %3, ptr %1, align 4, !tbaa !5
  %4 = getelementptr inbounds i8, ptr %0, i64 4
  %5 = load i32, ptr %4, align 4, !tbaa !5
  %6 = add nsw i32 %3, %5
  store i32 %6, ptr %1, align 4, !tbaa !5
  %7 = getelementptr inbounds i8, ptr %0, i64 8
  %8 = load i32, ptr %7, align 4, !tbaa !5
  %9 = add nsw i32 %6, %8
  store i32 %9, ptr %1, align 4, !tbaa !5
  %10 = getelementptr inbounds i8, ptr %0, i64 12
  %11 = load i32, ptr %10, align 4, !tbaa !5
  %12 = add nsw i32 %9, %11
  store i32 %12, ptr %1, align 4, !tbaa !5
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: readwrite) uwtable
define dso_local void @something_64_void(ptr nocapture noundef readonly %0, ptr nocapture noundef writeonly %1) local_unnamed_addr #1 {
  store i64 0, ptr %1, align 8, !tbaa !9
  %3 = load i64, ptr %0, align 8, !tbaa !9
  store i64 %3, ptr %1, align 8, !tbaa !9
  %4 = getelementptr inbounds i8, ptr %0, i64 8
  %5 = load i64, ptr %4, align 8, !tbaa !9
  %6 = add nsw i64 %3, %5
  store i64 %6, ptr %1, align 8, !tbaa !9
  %7 = getelementptr inbounds i8, ptr %0, i64 16
  %8 = load i64, ptr %7, align 8, !tbaa !9
  %9 = add nsw i64 %6, %8
  store i64 %9, ptr %1, align 8, !tbaa !9
  %10 = getelementptr inbounds i8, ptr %0, i64 24
  %11 = load i64, ptr %10, align 8, !tbaa !9
  %12 = add nsw i64 %9, %11
  store i64 %12, ptr %1, align 8, !tbaa !9
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: readwrite) uwtable
define dso_local void @vector_sum(ptr nocapture noundef readonly %0, ptr nocapture noundef readonly %1, ptr nocapture noundef writeonly %2) local_unnamed_addr #1 {
  %4 = load i32, ptr %0, align 4, !tbaa !5
  %5 = load i32, ptr %1, align 4, !tbaa !5
  %6 = add nsw i32 %5, %4
  store i32 %6, ptr %2, align 4, !tbaa !5
  %7 = getelementptr inbounds i8, ptr %0, i64 4
  %8 = load i32, ptr %7, align 4, !tbaa !5
  %9 = getelementptr inbounds i8, ptr %1, i64 4
  %10 = load i32, ptr %9, align 4, !tbaa !5
  %11 = add nsw i32 %10, %8
  %12 = getelementptr inbounds i8, ptr %2, i64 4
  store i32 %11, ptr %12, align 4, !tbaa !5
  %13 = getelementptr inbounds i8, ptr %0, i64 8
  %14 = load i32, ptr %13, align 4, !tbaa !5
  %15 = getelementptr inbounds i8, ptr %1, i64 8
  %16 = load i32, ptr %15, align 4, !tbaa !5
  %17 = add nsw i32 %16, %14
  %18 = getelementptr inbounds i8, ptr %2, i64 8
  store i32 %17, ptr %18, align 4, !tbaa !5
  %19 = getelementptr inbounds i8, ptr %0, i64 12
  %20 = load i32, ptr %19, align 4, !tbaa !5
  %21 = getelementptr inbounds i8, ptr %1, i64 12
  %22 = load i32, ptr %21, align 4, !tbaa !5
  %23 = add nsw i32 %22, %20
  %24 = getelementptr inbounds i8, ptr %2, i64 12
  store i32 %23, ptr %24, align 4, !tbaa !5
  %25 = getelementptr inbounds i8, ptr %0, i64 16
  %26 = load i32, ptr %25, align 4, !tbaa !5
  %27 = getelementptr inbounds i8, ptr %1, i64 16
  %28 = load i32, ptr %27, align 4, !tbaa !5
  %29 = add nsw i32 %28, %26
  %30 = getelementptr inbounds i8, ptr %2, i64 16
  store i32 %29, ptr %30, align 4, !tbaa !5
  %31 = getelementptr inbounds i8, ptr %0, i64 20
  %32 = load i32, ptr %31, align 4, !tbaa !5
  %33 = getelementptr inbounds i8, ptr %1, i64 20
  %34 = load i32, ptr %33, align 4, !tbaa !5
  %35 = add nsw i32 %34, %32
  %36 = getelementptr inbounds i8, ptr %2, i64 20
  store i32 %35, ptr %36, align 4, !tbaa !5
  %37 = getelementptr inbounds i8, ptr %0, i64 24
  %38 = load i32, ptr %37, align 4, !tbaa !5
  %39 = getelementptr inbounds i8, ptr %1, i64 24
  %40 = load i32, ptr %39, align 4, !tbaa !5
  %41 = add nsw i32 %40, %38
  %42 = getelementptr inbounds i8, ptr %2, i64 24
  store i32 %41, ptr %42, align 4, !tbaa !5
  %43 = getelementptr inbounds i8, ptr %0, i64 28
  %44 = load i32, ptr %43, align 4, !tbaa !5
  %45 = getelementptr inbounds i8, ptr %1, i64 28
  %46 = load i32, ptr %45, align 4, !tbaa !5
  %47 = add nsw i32 %46, %44
  %48 = getelementptr inbounds i8, ptr %2, i64 28
  store i32 %47, ptr %48, align 4, !tbaa !5
  %49 = getelementptr inbounds i8, ptr %0, i64 32
  %50 = load i32, ptr %49, align 4, !tbaa !5
  %51 = getelementptr inbounds i8, ptr %1, i64 32
  %52 = load i32, ptr %51, align 4, !tbaa !5
  %53 = add nsw i32 %52, %50
  %54 = getelementptr inbounds i8, ptr %2, i64 32
  store i32 %53, ptr %54, align 4, !tbaa !5
  %55 = getelementptr inbounds i8, ptr %0, i64 36
  %56 = load i32, ptr %55, align 4, !tbaa !5
  %57 = getelementptr inbounds i8, ptr %1, i64 36
  %58 = load i32, ptr %57, align 4, !tbaa !5
  %59 = add nsw i32 %58, %56
  %60 = getelementptr inbounds i8, ptr %2, i64 36
  store i32 %59, ptr %60, align 4, !tbaa !5
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: readwrite) uwtable
define dso_local void @vector_sum_restrict(ptr noalias nocapture noundef readonly %0, ptr noalias nocapture noundef readonly %1, ptr noalias nocapture noundef writeonly %2) local_unnamed_addr #1 {
  %4 = load <4 x i32>, ptr %0, align 4, !tbaa !5
  %5 = load <4 x i32>, ptr %1, align 4, !tbaa !5
  %6 = add nsw <4 x i32> %5, %4
  store <4 x i32> %6, ptr %2, align 4, !tbaa !5
  %7 = getelementptr inbounds i8, ptr %0, i64 16
  %8 = getelementptr inbounds i8, ptr %1, i64 16
  %9 = getelementptr inbounds i8, ptr %2, i64 16
  %10 = load <4 x i32>, ptr %7, align 4, !tbaa !5
  %11 = load <4 x i32>, ptr %8, align 4, !tbaa !5
  %12 = add nsw <4 x i32> %11, %10
  store <4 x i32> %12, ptr %9, align 4, !tbaa !5
  %13 = getelementptr inbounds i8, ptr %0, i64 32
  %14 = getelementptr inbounds i8, ptr %1, i64 32
  %15 = getelementptr inbounds i8, ptr %2, i64 32
  %16 = load <2 x i32>, ptr %13, align 4, !tbaa !5
  %17 = load <2 x i32>, ptr %14, align 4, !tbaa !5
  %18 = add nsw <2 x i32> %17, %16
  store <2 x i32> %18, ptr %15, align 4, !tbaa !5
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.vector.reduce.add.v4i64(<4 x i64>) #2

attributes #0 = { mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: read) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: readwrite) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 19.1.7"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = !{!10, !10, i64 0}
!10 = !{!"long", !7, i64 0}
