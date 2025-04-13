; ModuleID = 'vector_starting.rl'
source_filename = "vector_starting.rl"
target datalayout = "S128-e-f80:128-i128:128-i64:64-p272:64:64:64:64-p271:32:32:32:32-f128:128-f16:16-p270:32:32:32:32-f64:64-i32:32-i16:16-i8:8-i1:8-p0:64:64:64:64"
target triple = "x86_64-unknown-linux-gnu"

@str_7 = internal constant [60 x i8] c"vector_starting.rl:28:24 error: Out of bounds array access.\0A"
@str_6 = internal constant [60 x i8] c"vector_starting.rl:28:17 error: Out of bounds array access.\0A"
@str_5 = internal constant [60 x i8] c"vector_starting.rl:28:10 error: Out of bounds array access.\0A"
@str_4 = internal constant [60 x i8] c"vector_starting.rl:21:24 error: Out of bounds array access.\0A"
@str_3 = internal constant [60 x i8] c"vector_starting.rl:21:17 error: Out of bounds array access.\0A"
@str_2 = internal constant [60 x i8] c"vector_starting.rl:21:10 error: Out of bounds array access.\0A"
@str_1 = internal constant [60 x i8] c"vector_starting.rl:12:18 error: Out of bounds array access.\0A"
@str_0 = internal constant [59 x i8] c"vector_starting.rl:5:18 error: Out of bounds array access.\0A"

declare ptr @malloc(i64)

declare i32 @puts(ptr)

declare void @free(ptr)

define void @rl_m_init__int64_t_10(ptr %0) {
  call void @llvm.memset.p0.i64(ptr %0, i8 0, i64 80, i1 false)
  ret void
}

define void @rl_m_assign__int64_t_10_int64_t_10(ptr %0, ptr %1) {
  call void @llvm.memmove.p0.p0.i64(ptr %0, ptr %1, i64 80, i1 false)
  ret void
}

define void @rl_red_void__int64_t_4_int64_t(ptr %0, ptr %1) {
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

define void @rl_red__int64_t_4_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  %5 = alloca i64, i64 1, align 8
  store i64 0, ptr %5, align 8
  store i64 0, ptr %4, align 8
  br label %6

6:                                                ; preds = %24, %2
  %7 = load i64, ptr %4, align 8
  %8 = icmp ne i64 %7, 4
  %9 = zext i1 %8 to i8
  %10 = icmp ne i8 %9, 0
  br i1 %10, label %11, label %32

11:                                               ; preds = %6
  %12 = load i64, ptr %4, align 8
  %13 = icmp sge i64 %12, 4
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
  %26 = getelementptr [4 x i64], ptr %1, i32 0, i64 %25
  %27 = load i64, ptr %5, align 8
  %28 = load i64, ptr %26, align 8
  %29 = add i64 %27, %28
  store i64 %29, ptr %5, align 8
  %30 = load i64, ptr %4, align 8
  %31 = add i64 %30, 1
  store i64 %31, ptr %4, align 8
  br label %6

32:                                               ; preds = %6
  store i64 0, ptr %3, align 8
  %33 = load i64, ptr %5, align 8
  store i64 %33, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_vector_sum__int64_t_10_int64_t_10_r_int64_t_10(ptr %0, ptr %1, ptr %2) {
  %4 = alloca [10 x i64], i64 1, align 8
  %5 = alloca i64, i64 1, align 8
  %6 = alloca [10 x i64], i64 1, align 8
  call void @rl_m_init__int64_t_10(ptr %6)
  store i64 0, ptr %5, align 8
  br label %7

7:                                                ; preds = %55, %3
  %8 = load i64, ptr %5, align 8
  %9 = icmp slt i64 %8, 10
  %10 = zext i1 %9 to i8
  %11 = icmp ne i8 %10, 0
  br i1 %11, label %12, label %63

12:                                               ; preds = %7
  %13 = load i64, ptr %5, align 8
  %14 = icmp sge i64 %13, 10
  %15 = zext i1 %14 to i8
  %16 = load i64, ptr %5, align 8
  %17 = icmp slt i64 %16, 0
  %18 = zext i1 %17 to i8
  %19 = or i8 %15, %18
  %20 = icmp eq i8 %19, 0
  %21 = zext i1 %20 to i8
  %22 = icmp ne i8 %21, 0
  br i1 %22, label %25, label %23

23:                                               ; preds = %12
  %24 = call i32 @puts(ptr @str_2)
  call void @llvm.trap()
  ret void

25:                                               ; preds = %12
  %26 = load i64, ptr %5, align 8
  %27 = getelementptr [10 x i64], ptr %6, i32 0, i64 %26
  %28 = load i64, ptr %5, align 8
  %29 = icmp sge i64 %28, 10
  %30 = zext i1 %29 to i8
  %31 = load i64, ptr %5, align 8
  %32 = icmp slt i64 %31, 0
  %33 = zext i1 %32 to i8
  %34 = or i8 %30, %33
  %35 = icmp eq i8 %34, 0
  %36 = zext i1 %35 to i8
  %37 = icmp ne i8 %36, 0
  br i1 %37, label %40, label %38

38:                                               ; preds = %25
  %39 = call i32 @puts(ptr @str_3)
  call void @llvm.trap()
  ret void

40:                                               ; preds = %25
  %41 = load i64, ptr %5, align 8
  %42 = getelementptr [10 x i64], ptr %1, i32 0, i64 %41
  %43 = load i64, ptr %5, align 8
  %44 = icmp sge i64 %43, 10
  %45 = zext i1 %44 to i8
  %46 = load i64, ptr %5, align 8
  %47 = icmp slt i64 %46, 0
  %48 = zext i1 %47 to i8
  %49 = or i8 %45, %48
  %50 = icmp eq i8 %49, 0
  %51 = zext i1 %50 to i8
  %52 = icmp ne i8 %51, 0
  br i1 %52, label %55, label %53

53:                                               ; preds = %40
  %54 = call i32 @puts(ptr @str_4)
  call void @llvm.trap()
  ret void

55:                                               ; preds = %40
  %56 = load i64, ptr %5, align 8
  %57 = getelementptr [10 x i64], ptr %2, i32 0, i64 %56
  %58 = load i64, ptr %42, align 8
  %59 = load i64, ptr %57, align 8
  %60 = add i64 %58, %59
  store i64 %60, ptr %27, align 8
  %61 = load i64, ptr %5, align 8
  %62 = add i64 %61, 1
  store i64 %62, ptr %5, align 8
  br label %7

63:                                               ; preds = %7
  call void @rl_m_init__int64_t_10(ptr %4)
  call void @rl_m_assign__int64_t_10_int64_t_10(ptr %4, ptr %6)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 80, i1 false)
  ret void
}

define void @rl_vector_sum_void__int64_t_10_int64_t_10_int64_t_10(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i64, i64 1, align 8
  store i64 0, ptr %4, align 8
  br label %5

5:                                                ; preds = %53, %3
  %6 = load i64, ptr %4, align 8
  %7 = icmp slt i64 %6, 10
  %8 = zext i1 %7 to i8
  %9 = icmp ne i8 %8, 0
  br i1 %9, label %10, label %61

10:                                               ; preds = %5
  %11 = load i64, ptr %4, align 8
  %12 = icmp sge i64 %11, 10
  %13 = zext i1 %12 to i8
  %14 = load i64, ptr %4, align 8
  %15 = icmp slt i64 %14, 0
  %16 = zext i1 %15 to i8
  %17 = or i8 %13, %16
  %18 = icmp eq i8 %17, 0
  %19 = zext i1 %18 to i8
  %20 = icmp ne i8 %19, 0
  br i1 %20, label %23, label %21

21:                                               ; preds = %10
  %22 = call i32 @puts(ptr @str_5)
  call void @llvm.trap()
  ret void

23:                                               ; preds = %10
  %24 = load i64, ptr %4, align 8
  %25 = getelementptr [10 x i64], ptr %2, i32 0, i64 %24
  %26 = load i64, ptr %4, align 8
  %27 = icmp sge i64 %26, 10
  %28 = zext i1 %27 to i8
  %29 = load i64, ptr %4, align 8
  %30 = icmp slt i64 %29, 0
  %31 = zext i1 %30 to i8
  %32 = or i8 %28, %31
  %33 = icmp eq i8 %32, 0
  %34 = zext i1 %33 to i8
  %35 = icmp ne i8 %34, 0
  br i1 %35, label %38, label %36

36:                                               ; preds = %23
  %37 = call i32 @puts(ptr @str_6)
  call void @llvm.trap()
  ret void

38:                                               ; preds = %23
  %39 = load i64, ptr %4, align 8
  %40 = getelementptr [10 x i64], ptr %0, i32 0, i64 %39
  %41 = load i64, ptr %4, align 8
  %42 = icmp sge i64 %41, 10
  %43 = zext i1 %42 to i8
  %44 = load i64, ptr %4, align 8
  %45 = icmp slt i64 %44, 0
  %46 = zext i1 %45 to i8
  %47 = or i8 %43, %46
  %48 = icmp eq i8 %47, 0
  %49 = zext i1 %48 to i8
  %50 = icmp ne i8 %49, 0
  br i1 %50, label %53, label %51

51:                                               ; preds = %38
  %52 = call i32 @puts(ptr @str_7)
  call void @llvm.trap()
  ret void

53:                                               ; preds = %38
  %54 = load i64, ptr %4, align 8
  %55 = getelementptr [10 x i64], ptr %1, i32 0, i64 %54
  %56 = load i64, ptr %40, align 8
  %57 = load i64, ptr %55, align 8
  %58 = add i64 %56, %57
  store i64 %58, ptr %25, align 8
  %59 = load i64, ptr %4, align 8
  %60 = add i64 %59, 1
  store i64 %60, ptr %4, align 8
  br label %5

61:                                               ; preds = %5
  ret void
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #0

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i64(ptr nocapture writeonly, ptr nocapture readonly, i64, i1 immarg) #1

; Function Attrs: cold noreturn nounwind memory(inaccessiblemem: write)
declare void @llvm.trap() #2

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #1

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { cold noreturn nounwind memory(inaccessiblemem: write) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
