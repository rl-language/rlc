; ModuleID = 'rlc-vector-starting.ll'
source_filename = "vector_starting.rl"
target datalayout = "e-S128-i16:16-i8:8-i32:32-f128:128-f16:16-p270:32:32:32:32-f64:64-p271:32:32:32:32-p272:64:64:64:64-i64:64-i128:128-f80:128-i1:8-p0:64:64:64:64"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write)
define void @rl_m_init__int64_t_10(ptr nocapture writeonly %0) local_unnamed_addr #0 {
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(80) %0, i8 0, i64 80, i1 false)
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_something__int64_t_4_int64_t(ptr nocapture readonly %0, ptr nocapture writeonly %1) local_unnamed_addr #1 {
  store i64 0, ptr %1, align 8
  %3 = load i64, ptr %0, align 8
  store i64 %3, ptr %1, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load i64, ptr %4, align 8
  %6 = add i64 %5, %3
  store i64 %6, ptr %1, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load i64, ptr %7, align 8
  %9 = add i64 %8, %6
  store i64 %9, ptr %1, align 8
  %10 = getelementptr i8, ptr %0, i64 24
  %11 = load i64, ptr %10, align 8
  %12 = add i64 %11, %9
  store i64 %12, ptr %1, align 8
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

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
