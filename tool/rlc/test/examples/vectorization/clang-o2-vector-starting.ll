; ModuleID = 'vector_starting.c'
source_filename = "vector_starting.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) uwtable
define dso_local i32 @something_32(ptr nocapture noundef readonly %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %0 = load <4 x i32>, ptr %a, align 4, !tbaa !5
  %1 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %0)
  ret i32 %1
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) uwtable
define dso_local i64 @something_64(ptr nocapture noundef readonly %a, i64 noundef %b) local_unnamed_addr #0 {
entry:
  %0 = load <4 x i64>, ptr %a, align 8, !tbaa !9
  %1 = tail call i64 @llvm.vector.reduce.add.v4i64(<4 x i64> %0)
  ret i64 %1
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) uwtable
define dso_local void @something_32_void(ptr nocapture noundef readonly %a, ptr nocapture noundef writeonly %b) local_unnamed_addr #1 {
entry:
  store i32 0, ptr %b, align 4, !tbaa !5
  %0 = load i32, ptr %a, align 4, !tbaa !5
  store i32 %0, ptr %b, align 4, !tbaa !5
  %arrayidx.1 = getelementptr inbounds i8, ptr %a, i64 4
  %1 = load i32, ptr %arrayidx.1, align 4, !tbaa !5
  %add.1 = add nsw i32 %0, %1
  store i32 %add.1, ptr %b, align 4, !tbaa !5
  %arrayidx.2 = getelementptr inbounds i8, ptr %a, i64 8
  %2 = load i32, ptr %arrayidx.2, align 4, !tbaa !5
  %add.2 = add nsw i32 %add.1, %2
  store i32 %add.2, ptr %b, align 4, !tbaa !5
  %arrayidx.3 = getelementptr inbounds i8, ptr %a, i64 12
  %3 = load i32, ptr %arrayidx.3, align 4, !tbaa !5
  %add.3 = add nsw i32 %add.2, %3
  store i32 %add.3, ptr %b, align 4, !tbaa !5
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) uwtable
define dso_local void @something_64_void(ptr nocapture noundef readonly %a, ptr nocapture noundef writeonly %b) local_unnamed_addr #1 {
entry:
  store i64 0, ptr %b, align 8, !tbaa !9
  %0 = load i64, ptr %a, align 8, !tbaa !9
  store i64 %0, ptr %b, align 8, !tbaa !9
  %arrayidx.1 = getelementptr inbounds i8, ptr %a, i64 8
  %1 = load i64, ptr %arrayidx.1, align 8, !tbaa !9
  %add.1 = add nsw i64 %0, %1
  store i64 %add.1, ptr %b, align 8, !tbaa !9
  %arrayidx.2 = getelementptr inbounds i8, ptr %a, i64 16
  %2 = load i64, ptr %arrayidx.2, align 8, !tbaa !9
  %add.2 = add nsw i64 %add.1, %2
  store i64 %add.2, ptr %b, align 8, !tbaa !9
  %arrayidx.3 = getelementptr inbounds i8, ptr %a, i64 24
  %3 = load i64, ptr %arrayidx.3, align 8, !tbaa !9
  %add.3 = add nsw i64 %add.2, %3
  store i64 %add.3, ptr %b, align 8, !tbaa !9
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) uwtable
define dso_local void @vector_sum(ptr nocapture noundef readonly %a, ptr nocapture noundef readonly %b, ptr nocapture noundef writeonly %c) local_unnamed_addr #1 {
entry:
  %0 = load i32, ptr %a, align 4, !tbaa !5
  %1 = load i32, ptr %b, align 4, !tbaa !5
  %add = add nsw i32 %1, %0
  store i32 %add, ptr %c, align 4, !tbaa !5
  %arrayidx.1 = getelementptr inbounds i8, ptr %a, i64 4
  %2 = load i32, ptr %arrayidx.1, align 4, !tbaa !5
  %arrayidx2.1 = getelementptr inbounds i8, ptr %b, i64 4
  %3 = load i32, ptr %arrayidx2.1, align 4, !tbaa !5
  %add.1 = add nsw i32 %3, %2
  %arrayidx4.1 = getelementptr inbounds i8, ptr %c, i64 4
  store i32 %add.1, ptr %arrayidx4.1, align 4, !tbaa !5
  %arrayidx.2 = getelementptr inbounds i8, ptr %a, i64 8
  %4 = load i32, ptr %arrayidx.2, align 4, !tbaa !5
  %arrayidx2.2 = getelementptr inbounds i8, ptr %b, i64 8
  %5 = load i32, ptr %arrayidx2.2, align 4, !tbaa !5
  %add.2 = add nsw i32 %5, %4
  %arrayidx4.2 = getelementptr inbounds i8, ptr %c, i64 8
  store i32 %add.2, ptr %arrayidx4.2, align 4, !tbaa !5
  %arrayidx.3 = getelementptr inbounds i8, ptr %a, i64 12
  %6 = load i32, ptr %arrayidx.3, align 4, !tbaa !5
  %arrayidx2.3 = getelementptr inbounds i8, ptr %b, i64 12
  %7 = load i32, ptr %arrayidx2.3, align 4, !tbaa !5
  %add.3 = add nsw i32 %7, %6
  %arrayidx4.3 = getelementptr inbounds i8, ptr %c, i64 12
  store i32 %add.3, ptr %arrayidx4.3, align 4, !tbaa !5
  %arrayidx.4 = getelementptr inbounds i8, ptr %a, i64 16
  %8 = load i32, ptr %arrayidx.4, align 4, !tbaa !5
  %arrayidx2.4 = getelementptr inbounds i8, ptr %b, i64 16
  %9 = load i32, ptr %arrayidx2.4, align 4, !tbaa !5
  %add.4 = add nsw i32 %9, %8
  %arrayidx4.4 = getelementptr inbounds i8, ptr %c, i64 16
  store i32 %add.4, ptr %arrayidx4.4, align 4, !tbaa !5
  %arrayidx.5 = getelementptr inbounds i8, ptr %a, i64 20
  %10 = load i32, ptr %arrayidx.5, align 4, !tbaa !5
  %arrayidx2.5 = getelementptr inbounds i8, ptr %b, i64 20
  %11 = load i32, ptr %arrayidx2.5, align 4, !tbaa !5
  %add.5 = add nsw i32 %11, %10
  %arrayidx4.5 = getelementptr inbounds i8, ptr %c, i64 20
  store i32 %add.5, ptr %arrayidx4.5, align 4, !tbaa !5
  %arrayidx.6 = getelementptr inbounds i8, ptr %a, i64 24
  %12 = load i32, ptr %arrayidx.6, align 4, !tbaa !5
  %arrayidx2.6 = getelementptr inbounds i8, ptr %b, i64 24
  %13 = load i32, ptr %arrayidx2.6, align 4, !tbaa !5
  %add.6 = add nsw i32 %13, %12
  %arrayidx4.6 = getelementptr inbounds i8, ptr %c, i64 24
  store i32 %add.6, ptr %arrayidx4.6, align 4, !tbaa !5
  %arrayidx.7 = getelementptr inbounds i8, ptr %a, i64 28
  %14 = load i32, ptr %arrayidx.7, align 4, !tbaa !5
  %arrayidx2.7 = getelementptr inbounds i8, ptr %b, i64 28
  %15 = load i32, ptr %arrayidx2.7, align 4, !tbaa !5
  %add.7 = add nsw i32 %15, %14
  %arrayidx4.7 = getelementptr inbounds i8, ptr %c, i64 28
  store i32 %add.7, ptr %arrayidx4.7, align 4, !tbaa !5
  %arrayidx.8 = getelementptr inbounds i8, ptr %a, i64 32
  %16 = load i32, ptr %arrayidx.8, align 4, !tbaa !5
  %arrayidx2.8 = getelementptr inbounds i8, ptr %b, i64 32
  %17 = load i32, ptr %arrayidx2.8, align 4, !tbaa !5
  %add.8 = add nsw i32 %17, %16
  %arrayidx4.8 = getelementptr inbounds i8, ptr %c, i64 32
  store i32 %add.8, ptr %arrayidx4.8, align 4, !tbaa !5
  %arrayidx.9 = getelementptr inbounds i8, ptr %a, i64 36
  %18 = load i32, ptr %arrayidx.9, align 4, !tbaa !5
  %arrayidx2.9 = getelementptr inbounds i8, ptr %b, i64 36
  %19 = load i32, ptr %arrayidx2.9, align 4, !tbaa !5
  %add.9 = add nsw i32 %19, %18
  %arrayidx4.9 = getelementptr inbounds i8, ptr %c, i64 36
  store i32 %add.9, ptr %arrayidx4.9, align 4, !tbaa !5
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) uwtable
define dso_local void @vector_sum_restrict(ptr noalias nocapture noundef readonly %a, ptr noalias nocapture noundef readonly %b, ptr noalias nocapture noundef writeonly %c) local_unnamed_addr #1 {
entry:
  %0 = load <4 x i32>, ptr %a, align 4, !tbaa !5
  %1 = load <4 x i32>, ptr %b, align 4, !tbaa !5
  %2 = add nsw <4 x i32> %1, %0
  store <4 x i32> %2, ptr %c, align 4, !tbaa !5
  %arrayidx.4 = getelementptr inbounds i8, ptr %a, i64 16
  %arrayidx2.4 = getelementptr inbounds i8, ptr %b, i64 16
  %arrayidx4.4 = getelementptr inbounds i8, ptr %c, i64 16
  %3 = load <4 x i32>, ptr %arrayidx.4, align 4, !tbaa !5
  %4 = load <4 x i32>, ptr %arrayidx2.4, align 4, !tbaa !5
  %5 = add nsw <4 x i32> %4, %3
  store <4 x i32> %5, ptr %arrayidx4.4, align 4, !tbaa !5
  %arrayidx.8 = getelementptr inbounds i8, ptr %a, i64 32
  %arrayidx2.8 = getelementptr inbounds i8, ptr %b, i64 32
  %arrayidx4.8 = getelementptr inbounds i8, ptr %c, i64 32
  %6 = load <2 x i32>, ptr %arrayidx.8, align 4, !tbaa !5
  %7 = load <2 x i32>, ptr %arrayidx2.8, align 4, !tbaa !5
  %8 = add nsw <2 x i32> %7, %6
  store <2 x i32> %8, ptr %arrayidx4.8, align 4, !tbaa !5
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.vector.reduce.add.v4i64(<4 x i64>) #2

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 19.1.7 (https://github.com/llvm/llvm-project.git cd708029e0b2869e80abe31ddb175f7c35361f90)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = !{!10, !10, i64 0}
!10 = !{!"long", !7, i64 0}
