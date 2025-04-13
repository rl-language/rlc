; ModuleID = 'rlc-vector-starting.ll'
source_filename = "vector_starting.rl"
target datalayout = "S128-e-i1:8-p0:64:64:64:64-f80:128-i128:128-i64:64-p271:32:32:32:32-p272:64:64:64:64-f128:128-f16:16-p270:32:32:32:32-f64:64-i32:32-i16:16-i8:8"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write)
define void @rl_m_init__int64_t_10(ptr nocapture writeonly %0) local_unnamed_addr #0 {
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(80) %0, i8 0, i64 80, i1 false)
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_red_void__int64_t_4_int64_t(ptr nocapture readonly %0, ptr noalias nocapture writeonly %1) local_unnamed_addr #1 {
  %3 = load <4 x i64>, ptr %0, align 8
  %4 = tail call i64 @llvm.vector.reduce.add.v4i64(<4 x i64> %3)
  store i64 %4, ptr %1, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_red__int64_t_4_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #1 {
  %3 = load <4 x i64>, ptr %1, align 8
  %4 = tail call i64 @llvm.vector.reduce.add.v4i64(<4 x i64> %3)
  store i64 %4, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_vector_sum__int64_t_10_int64_t_10_r_int64_t_10(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #1 {
  %4 = getelementptr i8, ptr %1, i64 16
  %5 = getelementptr i8, ptr %2, i64 16
  %6 = getelementptr i8, ptr %1, i64 32
  %7 = getelementptr i8, ptr %2, i64 32
  %8 = getelementptr i8, ptr %1, i64 48
  %9 = getelementptr i8, ptr %2, i64 48
  %10 = getelementptr i8, ptr %1, i64 64
  %11 = getelementptr i8, ptr %2, i64 64
  %12 = load <2 x i64>, ptr %1, align 8
  %13 = load <2 x i64>, ptr %2, align 8
  %14 = add <2 x i64> %13, %12
  %.sroa.4.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 16
  %15 = load <2 x i64>, ptr %4, align 8
  %16 = load <2 x i64>, ptr %5, align 8
  %17 = add <2 x i64> %16, %15
  %.sroa.6.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 32
  %18 = load <2 x i64>, ptr %6, align 8
  %19 = load <2 x i64>, ptr %7, align 8
  %20 = add <2 x i64> %19, %18
  %.sroa.8.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 48
  %21 = load <2 x i64>, ptr %8, align 8
  %22 = load <2 x i64>, ptr %9, align 8
  %23 = add <2 x i64> %22, %21
  %.sroa.10.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 64
  %24 = load <2 x i64>, ptr %10, align 8
  %25 = load <2 x i64>, ptr %11, align 8
  %26 = add <2 x i64> %25, %24
  store <2 x i64> %14, ptr %0, align 1
  store <2 x i64> %17, ptr %.sroa.4.0..sroa_idx, align 1
  store <2 x i64> %20, ptr %.sroa.6.0..sroa_idx, align 1
  store <2 x i64> %23, ptr %.sroa.8.0..sroa_idx, align 1
  store <2 x i64> %26, ptr %.sroa.10.0..sroa_idx, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_vector_sum_void__int64_t_10_int64_t_10_int64_t_10(ptr nocapture readonly %0, ptr nocapture readonly %1, ptr nocapture writeonly %2) local_unnamed_addr #1 {
  %4 = load i64, ptr %0, align 8
  %5 = load i64, ptr %1, align 8
  %6 = add i64 %5, %4
  store i64 %6, ptr %2, align 8
  %7 = getelementptr i8, ptr %2, i64 8
  %8 = getelementptr i8, ptr %0, i64 8
  %9 = getelementptr i8, ptr %1, i64 8
  %10 = load i64, ptr %8, align 8
  %11 = load i64, ptr %9, align 8
  %12 = add i64 %11, %10
  store i64 %12, ptr %7, align 8
  %13 = getelementptr i8, ptr %2, i64 16
  %14 = getelementptr i8, ptr %0, i64 16
  %15 = getelementptr i8, ptr %1, i64 16
  %16 = load i64, ptr %14, align 8
  %17 = load i64, ptr %15, align 8
  %18 = add i64 %17, %16
  store i64 %18, ptr %13, align 8
  %19 = getelementptr i8, ptr %2, i64 24
  %20 = getelementptr i8, ptr %0, i64 24
  %21 = getelementptr i8, ptr %1, i64 24
  %22 = load i64, ptr %20, align 8
  %23 = load i64, ptr %21, align 8
  %24 = add i64 %23, %22
  store i64 %24, ptr %19, align 8
  %25 = getelementptr i8, ptr %2, i64 32
  %26 = getelementptr i8, ptr %0, i64 32
  %27 = getelementptr i8, ptr %1, i64 32
  %28 = load i64, ptr %26, align 8
  %29 = load i64, ptr %27, align 8
  %30 = add i64 %29, %28
  store i64 %30, ptr %25, align 8
  %31 = getelementptr i8, ptr %2, i64 40
  %32 = getelementptr i8, ptr %0, i64 40
  %33 = getelementptr i8, ptr %1, i64 40
  %34 = load i64, ptr %32, align 8
  %35 = load i64, ptr %33, align 8
  %36 = add i64 %35, %34
  store i64 %36, ptr %31, align 8
  %37 = getelementptr i8, ptr %2, i64 48
  %38 = getelementptr i8, ptr %0, i64 48
  %39 = getelementptr i8, ptr %1, i64 48
  %40 = load i64, ptr %38, align 8
  %41 = load i64, ptr %39, align 8
  %42 = add i64 %41, %40
  store i64 %42, ptr %37, align 8
  %43 = getelementptr i8, ptr %2, i64 56
  %44 = getelementptr i8, ptr %0, i64 56
  %45 = getelementptr i8, ptr %1, i64 56
  %46 = load i64, ptr %44, align 8
  %47 = load i64, ptr %45, align 8
  %48 = add i64 %47, %46
  store i64 %48, ptr %43, align 8
  %49 = getelementptr i8, ptr %2, i64 64
  %50 = getelementptr i8, ptr %0, i64 64
  %51 = getelementptr i8, ptr %1, i64 64
  %52 = load i64, ptr %50, align 8
  %53 = load i64, ptr %51, align 8
  %54 = add i64 %53, %52
  store i64 %54, ptr %49, align 8
  %55 = getelementptr i8, ptr %2, i64 72
  %56 = getelementptr i8, ptr %0, i64 72
  %57 = getelementptr i8, ptr %1, i64 72
  %58 = load i64, ptr %56, align 8
  %59 = load i64, ptr %57, align 8
  %60 = add i64 %59, %58
  store i64 %60, ptr %55, align 8
  ret void
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.vector.reduce.add.v4i64(<4 x i64>) #3

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
