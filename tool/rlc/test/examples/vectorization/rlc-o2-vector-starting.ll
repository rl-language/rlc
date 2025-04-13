; ModuleID = 'vector_starting.rl'
source_filename = "vector_starting.rl"
target datalayout = "S128-e-f80:128-p0:64:64:64:64-i1:8-i64:64-i128:128-f16:16-f128:128-p270:32:32:32:32-f64:64-p271:32:32:32:32-p272:64:64:64:64-i8:8-i16:16-i32:32"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write)
define void @rl_m_init__int64_t_10(ptr nocapture writeonly %0) local_unnamed_addr #0 {
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(80) %0, i8 0, i64 80, i1 false)
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_assign__int64_t_10_int64_t_10(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #1 {
  tail call void @llvm.memmove.p0.p0.i64(ptr noundef nonnull align 1 dereferenceable(80) %0, ptr noundef nonnull align 1 dereferenceable(80) %1, i64 80, i1 false)
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_red_void__int64_t_4_int64_t(ptr nocapture readonly %0, ptr nocapture writeonly %1) local_unnamed_addr #1 {
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
define void @rl_red__int64_t_4_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #1 {
  %3 = load <4 x i64>, ptr %1, align 8
  %4 = tail call i64 @llvm.vector.reduce.add.v4i64(<4 x i64> %3)
  store i64 %4, ptr %0, align 1
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
  %.sroa.33.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 8
  store i64 %11, ptr %.sroa.33.0..sroa_idx, align 1
  %.sroa.44.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 16
  store i64 %16, ptr %.sroa.44.0..sroa_idx, align 1
  %.sroa.55.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 24
  store i64 %21, ptr %.sroa.55.0..sroa_idx, align 1
  %.sroa.66.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 32
  store i64 %26, ptr %.sroa.66.0..sroa_idx, align 1
  %.sroa.77.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 40
  store i64 %31, ptr %.sroa.77.0..sroa_idx, align 1
  %.sroa.88.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 48
  store i64 %36, ptr %.sroa.88.0..sroa_idx, align 1
  %.sroa.99.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 56
  store i64 %41, ptr %.sroa.99.0..sroa_idx, align 1
  %.sroa.1010.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 64
  store i64 %46, ptr %.sroa.1010.0..sroa_idx, align 1
  %.sroa.1111.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 72
  store i64 %51, ptr %.sroa.1111.0..sroa_idx, align 1
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

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i64(ptr nocapture writeonly, ptr nocapture readonly, i64, i1 immarg) #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.vector.reduce.add.v4i64(<4 x i64>) #4

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #3 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #4 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
