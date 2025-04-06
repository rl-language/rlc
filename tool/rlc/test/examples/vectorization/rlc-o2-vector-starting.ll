; ModuleID = 'vector_starting.rl'
source_filename = "vector_starting.rl"
target datalayout = "e-S128-p271:32:32:32:32-p272:64:64:64:64-f128:128-f16:16-p270:32:32:32:32-f64:64-i32:32-i16:16-i8:8-i1:8-p0:64:64:64:64-f80:128-i128:128-i64:64"
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
  %4 = load i64, ptr %1, align 8
  %5 = load i64, ptr %2, align 8
  %6 = add i64 %5, %4
  %7 = getelementptr i8, ptr %1, i64 8
  %8 = getelementptr i8, ptr %2, i64 8
  %9 = load i64, ptr %7, align 8
  %10 = load i64, ptr %8, align 8
  %11 = add i64 %10, %9
  %12 = getelementptr i8, ptr %1, i64 16
  %13 = getelementptr i8, ptr %2, i64 16
  %14 = load i64, ptr %12, align 8
  %15 = load i64, ptr %13, align 8
  %16 = add i64 %15, %14
  %17 = getelementptr i8, ptr %1, i64 24
  %18 = getelementptr i8, ptr %2, i64 24
  %19 = load i64, ptr %17, align 8
  %20 = load i64, ptr %18, align 8
  %21 = add i64 %20, %19
  %22 = getelementptr i8, ptr %1, i64 32
  %23 = getelementptr i8, ptr %2, i64 32
  %24 = load i64, ptr %22, align 8
  %25 = load i64, ptr %23, align 8
  %26 = add i64 %25, %24
  %27 = getelementptr i8, ptr %1, i64 40
  %28 = getelementptr i8, ptr %2, i64 40
  %29 = load i64, ptr %27, align 8
  %30 = load i64, ptr %28, align 8
  %31 = add i64 %30, %29
  %32 = getelementptr i8, ptr %1, i64 48
  %33 = getelementptr i8, ptr %2, i64 48
  %34 = load i64, ptr %32, align 8
  %35 = load i64, ptr %33, align 8
  %36 = add i64 %35, %34
  %37 = getelementptr i8, ptr %1, i64 56
  %38 = getelementptr i8, ptr %2, i64 56
  %39 = load i64, ptr %37, align 8
  %40 = load i64, ptr %38, align 8
  %41 = add i64 %40, %39
  %42 = getelementptr i8, ptr %1, i64 64
  %43 = getelementptr i8, ptr %2, i64 64
  %44 = load i64, ptr %42, align 8
  %45 = load i64, ptr %43, align 8
  %46 = add i64 %45, %44
  %47 = getelementptr i8, ptr %1, i64 72
  %48 = getelementptr i8, ptr %2, i64 72
  %49 = load i64, ptr %47, align 8
  %50 = load i64, ptr %48, align 8
  %51 = add i64 %50, %49
  store i64 %6, ptr %0, align 1
  %.sroa.3.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 8
  store i64 %11, ptr %.sroa.3.0..sroa_idx, align 1
  %.sroa.4.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 16
  store i64 %16, ptr %.sroa.4.0..sroa_idx, align 1
  %.sroa.5.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 24
  store i64 %21, ptr %.sroa.5.0..sroa_idx, align 1
  %.sroa.6.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 32
  store i64 %26, ptr %.sroa.6.0..sroa_idx, align 1
  %.sroa.7.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 40
  store i64 %31, ptr %.sroa.7.0..sroa_idx, align 1
  %.sroa.8.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 48
  store i64 %36, ptr %.sroa.8.0..sroa_idx, align 1
  %.sroa.9.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 56
  store i64 %41, ptr %.sroa.9.0..sroa_idx, align 1
  %.sroa.10.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 64
  store i64 %46, ptr %.sroa.10.0..sroa_idx, align 1
  %.sroa.11.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 72
  store i64 %51, ptr %.sroa.11.0..sroa_idx, align 1
  ret void
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
