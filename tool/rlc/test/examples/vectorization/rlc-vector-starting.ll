; ModuleID = 'vector_starting.rl'
source_filename = "vector_starting.rl"
target datalayout = "e-S128-i16:16-i8:8-i32:32-f128:128-f16:16-p270:32:32:32:32-f64:64-p271:32:32:32:32-p272:64:64:64:64-i64:64-i128:128-f80:128-i1:8-p0:64:64:64:64"
target triple = "x86_64-unknown-linux-gnu"

@str_3 = internal constant [60 x i8] c"vector_starting.rl:13:24 error: Out of bounds array access.\0A"
@str_2 = internal constant [60 x i8] c"vector_starting.rl:13:17 error: Out of bounds array access.\0A"
@str_1 = internal constant [60 x i8] c"vector_starting.rl:13:10 error: Out of bounds array access.\0A"
@str_0 = internal constant [59 x i8] c"vector_starting.rl:5:18 error: Out of bounds array access.\0A"

declare ptr @malloc(i64)

declare i32 @puts(ptr)

declare void @free(ptr)

define void @rl_m_init__int64_t_10(ptr %0) {
  call void @llvm.memset.p0.i64(ptr %0, i8 0, i64 80, i1 false)
  ret void
}

define void @rl_something__int64_t_4_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  store i64 0, ptr %1, align 8
  store i64 0, ptr %3, align 8
  br label %4

4:                                                ; preds = %22, %2
  %5 = load i64, ptr %3, align 8
  %6 = icmp ne i64 %5, 4
  %7 = zext i1 %6 to i8
  %8 = icmp ne i8 %7, 0
  br i1 %8, label %9, label %30

9:                                                ; preds = %4
  %10 = load i64, ptr %3, align 8
  %11 = icmp sge i64 %10, 4
  %12 = zext i1 %11 to i8
  %13 = load i64, ptr %3, align 8
  %14 = icmp slt i64 %13, 0
  %15 = zext i1 %14 to i8
  %16 = or i8 %12, %15
  %17 = icmp eq i8 %16, 0
  %18 = zext i1 %17 to i8
  %19 = icmp ne i8 %18, 0
  br i1 %19, label %22, label %20

20:                                               ; preds = %9
  %21 = call i32 @puts(ptr @str_0)
  call void @llvm.trap()
  ret void

22:                                               ; preds = %9
  %23 = load i64, ptr %3, align 8
  %24 = getelementptr [4 x i64], ptr %0, i32 0, i64 %23
  %25 = load i64, ptr %1, align 8
  %26 = load i64, ptr %24, align 8
  %27 = add i64 %25, %26
  store i64 %27, ptr %1, align 8
  %28 = load i64, ptr %3, align 8
  %29 = add i64 %28, 1
  store i64 %29, ptr %3, align 8
  br label %4

30:                                               ; preds = %4
  ret void
}

define void @rl_vector_sum__int64_t_10_int64_t_10_r_int64_t_10(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i64, i64 1, align 8
  %5 = alloca [10 x i64], i64 1, align 8
  call void @rl_m_init__int64_t_10(ptr %5)
  store i64 0, ptr %4, align 8
  br label %6

6:                                                ; preds = %54, %3
  %7 = load i64, ptr %4, align 8
  %8 = icmp slt i64 %7, 10
  %9 = zext i1 %8 to i8
  %10 = icmp ne i8 %9, 0
  br i1 %10, label %11, label %62

11:                                               ; preds = %6
  %12 = load i64, ptr %4, align 8
  %13 = icmp sge i64 %12, 10
  %14 = zext i1 %13 to i8
  %15 = load i64, ptr %4, align 8
  %16 = icmp slt i64 %15, 0
  %17 = zext i1 %16 to i8
  %18 = or i8 %14, %17
  %19 = icmp eq i8 %18, 0
  %20 = zext i1 %19 to i8
  %21 = icmp ne i8 %20, 0
  br i1 %21, label %24, label %22

22:                                               ; preds = %11
  %23 = call i32 @puts(ptr @str_1)
  call void @llvm.trap()
  ret void

24:                                               ; preds = %11
  %25 = load i64, ptr %4, align 8
  %26 = getelementptr [10 x i64], ptr %5, i32 0, i64 %25
  %27 = load i64, ptr %4, align 8
  %28 = icmp sge i64 %27, 10
  %29 = zext i1 %28 to i8
  %30 = load i64, ptr %4, align 8
  %31 = icmp slt i64 %30, 0
  %32 = zext i1 %31 to i8
  %33 = or i8 %29, %32
  %34 = icmp eq i8 %33, 0
  %35 = zext i1 %34 to i8
  %36 = icmp ne i8 %35, 0
  br i1 %36, label %39, label %37

37:                                               ; preds = %24
  %38 = call i32 @puts(ptr @str_2)
  call void @llvm.trap()
  ret void

39:                                               ; preds = %24
  %40 = load i64, ptr %4, align 8
  %41 = getelementptr [10 x i64], ptr %1, i32 0, i64 %40
  %42 = load i64, ptr %4, align 8
  %43 = icmp sge i64 %42, 10
  %44 = zext i1 %43 to i8
  %45 = load i64, ptr %4, align 8
  %46 = icmp slt i64 %45, 0
  %47 = zext i1 %46 to i8
  %48 = or i8 %44, %47
  %49 = icmp eq i8 %48, 0
  %50 = zext i1 %49 to i8
  %51 = icmp ne i8 %50, 0
  br i1 %51, label %54, label %52

52:                                               ; preds = %39
  %53 = call i32 @puts(ptr @str_3)
  call void @llvm.trap()
  ret void

54:                                               ; preds = %39
  %55 = load i64, ptr %4, align 8
  %56 = getelementptr [10 x i64], ptr %2, i32 0, i64 %55
  %57 = load i64, ptr %41, align 8
  %58 = load i64, ptr %56, align 8
  %59 = add i64 %57, %58
  store i64 %59, ptr %26, align 8
  %60 = load i64, ptr %4, align 8
  %61 = add i64 %60, 1
  store i64 %61, ptr %4, align 8
  br label %6

62:                                               ; preds = %6
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 80, i1 false)
  ret void
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #0

; Function Attrs: cold noreturn nounwind memory(inaccessiblemem: write)
declare void @llvm.trap() #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #2

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #1 = { cold noreturn nounwind memory(inaccessiblemem: write) }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
