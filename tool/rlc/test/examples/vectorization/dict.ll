; ModuleID = 'dict.rl'
source_filename = "dict.rl"
target datalayout = "S128-e-p272:64:64:64:64-p271:32:32:32:32-i64:64-f80:128-i128:128-i1:8-p0:64:64:64:64-i16:16-i8:8-i32:32-f128:128-f16:16-p270:32:32:32:32-f64:64"
target triple = "x86_64-unknown-linux-gnu"

%Entry = type { i8, i64, i64, i64 }
%Dict = type { ptr, i64, i64, double }
%Range = type { i64 }
%Nothing = type { i8 }
%String = type { %Vector }
%Vector = type { ptr, i64, i64 }
%Vector.1 = type { ptr, i64, i64 }

@str_23 = internal constant [19 x i8] c"dicExc: key, value\00"
@str_22 = internal constant [3 x i8] c", \00"
@str_21 = internal constant [16 x i8] c"dic: key, value\00"
@str_20 = internal constant [15 x i8] c"REMOVAL FAILED\00"
@str_19 = internal constant [7 x i8] c"value \00"
@str_18 = internal constant [22 x i8] c"Found entry with key \00"
@str_17 = internal constant [2 x i8] c"\22\00"
@str_16 = internal constant [1 x i8] zeroinitializer
@str_15 = internal constant [5 x i8] c"true\00"
@str_14 = internal constant [6 x i8] c"false\00"
@str_13 = internal constant [3 x i8] c"  \00"
@str_12 = internal constant [83 x i8] c"../../../../../stdlib/collections/vector.rl:99:9 error: out of bound vector access\0A"
@str_11 = internal constant [84 x i8] c"../../../../../stdlib/collections/vector.rl:106:9 error: out of bound vector access\0A"
@str_10 = internal constant [84 x i8] c"../../../../../stdlib/collections/vector.rl:105:9 error: out of bound vector access\0A"
@str_9 = internal constant [84 x i8] c"../../../../../stdlib/collections/vector.rl:140:9 error: out of bound vector access\0A"
@str_8 = internal constant [121 x i8] c"../../../../../stdlib/collections/dictionary.rl:64:17 error: Maximum probe count exceeded - likely an implementation bug\0A"
@str_7 = internal constant [76 x i8] c"../../../../../stdlib/collections/dictionary.rl:119:17 error: key not found\0A"
@str_6 = internal constant [76 x i8] c"../../../../../stdlib/collections/dictionary.rl:125:21 error: key not found\0A"
@str_5 = internal constant [127 x i8] c"../../../../../stdlib/collections/dictionary.rl:113:17 error: GET: Maximum probe count exceeded - likely an implementation bug\0A"
@str_4 = internal constant [96 x i8] c"../../../../../stdlib/collections/dictionary.rl:103:13 error: key not found in empty dictionary\0A"
@str_3 = internal constant [132 x i8] c"../../../../../stdlib/collections/dictionary.rl:145:17 error: CONTAINS: Maximum probe count exceeded - likely an implementation bug\0A"
@str_2 = internal constant [130 x i8] c"../../../../../stdlib/collections/dictionary.rl:172:17 error: REMOVE: Maximum probe count exceeded - likely an implementation bug\0A"
@str_1 = internal constant [90 x i8] c"../../../../../stdlib/serialization/key_equal.rl:30:50 error: Out of bounds array access.\0A"
@str_0 = internal constant [90 x i8] c"../../../../../stdlib/serialization/key_equal.rl:30:33 error: Out of bounds array access.\0A"
@NUM_KEYS = internal constant i64 50

declare ptr @malloc(i64)

declare i32 @puts(ptr)

declare void @free(ptr)

define void @rl_m_init__EntryTint64_tTint64_tT(ptr %0) {
  %2 = getelementptr %Entry, ptr %0, i32 0, i32 0
  store i8 0, ptr %2, align 1
  %3 = getelementptr %Entry, ptr %0, i32 0, i32 1
  store i64 0, ptr %3, align 8
  %4 = getelementptr %Entry, ptr %0, i32 0, i32 2
  store i64 0, ptr %4, align 8
  %5 = getelementptr %Entry, ptr %0, i32 0, i32 3
  store i64 0, ptr %5, align 8
  ret void
}

define void @rl_m_assign__DictTint64_tTint64_tT_DictTint64_tTint64_tT(ptr %0, ptr %1) {
  %3 = getelementptr %Dict, ptr %0, i32 0, i32 0
  %4 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %5 = load ptr, ptr %4, align 8
  store ptr %5, ptr %3, align 8
  %6 = getelementptr %Dict, ptr %0, i32 0, i32 1
  %7 = getelementptr %Dict, ptr %1, i32 0, i32 1
  %8 = load i64, ptr %7, align 8
  store i64 %8, ptr %6, align 8
  %9 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %10 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %11 = load i64, ptr %10, align 8
  store i64 %11, ptr %9, align 8
  %12 = getelementptr %Dict, ptr %0, i32 0, i32 3
  %13 = getelementptr %Dict, ptr %1, i32 0, i32 3
  %14 = load double, ptr %13, align 8
  store double %14, ptr %12, align 8
  ret void
}

define void @rl_m_assign__EntryTint64_tTint64_tT_EntryTint64_tTint64_tT(ptr %0, ptr %1) {
  %3 = getelementptr %Entry, ptr %0, i32 0, i32 0
  %4 = getelementptr %Entry, ptr %1, i32 0, i32 0
  %5 = load i8, ptr %4, align 1
  store i8 %5, ptr %3, align 1
  %6 = getelementptr %Entry, ptr %0, i32 0, i32 1
  %7 = getelementptr %Entry, ptr %1, i32 0, i32 1
  %8 = load i64, ptr %7, align 8
  store i64 %8, ptr %6, align 8
  %9 = getelementptr %Entry, ptr %0, i32 0, i32 2
  %10 = getelementptr %Entry, ptr %1, i32 0, i32 2
  %11 = load i64, ptr %10, align 8
  store i64 %11, ptr %9, align 8
  %12 = getelementptr %Entry, ptr %0, i32 0, i32 3
  %13 = getelementptr %Entry, ptr %1, i32 0, i32 3
  %14 = load i64, ptr %13, align 8
  store i64 %14, ptr %12, align 8
  ret void
}

define void @rl_m_init__strlit(ptr %0) {
  call void @llvm.memset.p0.i64(ptr %0, i8 0, i64 8, i1 false)
  ret void
}

define void @rl_m_init__Range(ptr %0) {
  %2 = getelementptr %Range, ptr %0, i32 0, i32 0
  store i64 0, ptr %2, align 8
  ret void
}

define void @rl_m_init__Nothing(ptr %0) {
  %2 = getelementptr %Nothing, ptr %0, i32 0, i32 0
  store i8 0, ptr %2, align 1
  ret void
}

define void @rl_m_assign__String_String(ptr %0, ptr %1) {
  %3 = getelementptr %String, ptr %0, i32 0, i32 0
  %4 = getelementptr %String, ptr %1, i32 0, i32 0
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr %3, ptr %4)
  ret void
}

define void @rl_m_assign__Range_Range(ptr %0, ptr %1) {
  %3 = getelementptr %Range, ptr %0, i32 0, i32 0
  %4 = getelementptr %Range, ptr %1, i32 0, i32 0
  %5 = load i64, ptr %4, align 8
  store i64 %5, ptr %3, align 8
  ret void
}

define void @rl_m_assign__Nothing_Nothing(ptr %0, ptr %1) {
  %3 = getelementptr %Nothing, ptr %0, i32 0, i32 0
  %4 = getelementptr %Nothing, ptr %1, i32 0, i32 0
  %5 = load i8, ptr %4, align 1
  store i8 %5, ptr %3, align 1
  ret void
}

define void @rl_m_drop__String(ptr %0) {
  %2 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_drop__VectorTint8_tT(ptr %2)
  ret void
}

define void @rl_compute_hash__int64_t_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  store i64 0, ptr %4, align 8
  %5 = load i64, ptr %1, align 8
  store i64 %5, ptr %4, align 8
  %6 = load i64, ptr %4, align 8
  %7 = lshr i64 %6, 33
  %8 = load i64, ptr %4, align 8
  %9 = xor i64 %8, %7
  store i64 %9, ptr %4, align 8
  %10 = load i64, ptr %4, align 8
  %11 = mul i64 %10, 1099511628211
  store i64 %11, ptr %4, align 8
  %12 = load i64, ptr %4, align 8
  %13 = lshr i64 %12, 33
  %14 = load i64, ptr %4, align 8
  %15 = xor i64 %14, %13
  store i64 %15, ptr %4, align 8
  %16 = load i64, ptr %4, align 8
  %17 = mul i64 %16, 16777619
  store i64 %17, ptr %4, align 8
  %18 = load i64, ptr %4, align 8
  %19 = lshr i64 %18, 33
  %20 = load i64, ptr %4, align 8
  %21 = xor i64 %20, %19
  store i64 %21, ptr %4, align 8
  %22 = load i64, ptr %4, align 8
  %23 = and i64 %22, 9223372036854775807
  store i64 %23, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_compute_hash__double_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  %5 = load double, ptr %1, align 8
  %6 = fmul double %5, 1.000000e+06
  %7 = fptosi double %6 to i64
  store i64 %7, ptr %4, align 8
  %8 = load i64, ptr %4, align 8
  %9 = lshr i64 %8, 33
  %10 = load i64, ptr %4, align 8
  %11 = xor i64 %10, %9
  store i64 %11, ptr %4, align 8
  %12 = load i64, ptr %4, align 8
  %13 = mul i64 %12, 1099511628211
  store i64 %13, ptr %4, align 8
  %14 = load i64, ptr %4, align 8
  %15 = lshr i64 %14, 33
  %16 = load i64, ptr %4, align 8
  %17 = xor i64 %16, %15
  store i64 %17, ptr %4, align 8
  %18 = load i64, ptr %4, align 8
  %19 = mul i64 %18, 16777619
  store i64 %19, ptr %4, align 8
  %20 = load i64, ptr %4, align 8
  %21 = lshr i64 %20, 33
  %22 = load i64, ptr %4, align 8
  %23 = xor i64 %22, %21
  store i64 %23, ptr %4, align 8
  %24 = load i64, ptr %4, align 8
  %25 = and i64 %24, 9223372036854775807
  store i64 %25, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_compute_hash__bool_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  %5 = load i8, ptr %1, align 1
  %6 = icmp ne i8 %5, 0
  br i1 %6, label %8, label %7

7:                                                ; preds = %2
  store i64 2023011127830240574, ptr %4, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 8, i1 false)
  ret void

8:                                                ; preds = %2
  store i64 1321005721090711325, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_compute_hash__int8_t_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  %5 = load i8, ptr %1, align 1
  %6 = sext i8 %5 to i64
  %7 = and i64 %6, 255
  store i64 %7, ptr %4, align 8
  %8 = load i64, ptr %4, align 8
  %9 = shl i64 %8, 16
  %10 = load i64, ptr %4, align 8
  %11 = xor i64 %10, %9
  %12 = mul i64 %11, 72955717
  store i64 %12, ptr %4, align 8
  %13 = load i64, ptr %4, align 8
  %14 = lshr i64 %13, 16
  %15 = load i64, ptr %4, align 8
  %16 = xor i64 %15, %14
  %17 = mul i64 %16, 72955717
  store i64 %17, ptr %4, align 8
  %18 = load i64, ptr %4, align 8
  %19 = shl i64 %18, 16
  %20 = load i64, ptr %4, align 8
  %21 = xor i64 %20, %19
  %22 = mul i64 %21, 72955717
  store i64 %22, ptr %4, align 8
  %23 = load i64, ptr %4, align 8
  %24 = and i64 %23, 9223372036854775807
  store i64 %24, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_compute_hash__String_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca ptr, i64 1, align 8
  %5 = alloca i64, i64 1, align 8
  %6 = alloca i64, i64 1, align 8
  %7 = alloca i64, i64 1, align 8
  store i64 2166136261, ptr %7, align 8
  store i64 0, ptr %6, align 8
  br label %8

8:                                                ; preds = %14, %2
  call void @rl_m_size__String_r_int64_t(ptr %5, ptr %1)
  %9 = load i64, ptr %6, align 8
  %10 = load i64, ptr %5, align 8
  %11 = icmp slt i64 %9, %10
  %12 = zext i1 %11 to i8
  %13 = icmp ne i8 %12, 0
  br i1 %13, label %14, label %25

14:                                               ; preds = %8
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %4, ptr %1, ptr %6)
  %15 = load ptr, ptr %4, align 8
  %16 = load i8, ptr %15, align 1
  %17 = sext i8 %16 to i64
  %18 = load i64, ptr %7, align 8
  %19 = xor i64 %18, %17
  store i64 %19, ptr %7, align 8
  %20 = load i64, ptr %7, align 8
  %21 = mul i64 %20, 16777619
  %22 = and i64 %21, 9223372036854775807
  store i64 %22, ptr %7, align 8
  %23 = load i64, ptr %6, align 8
  %24 = add i64 %23, 1
  store i64 %24, ptr %6, align 8
  br label %8

25:                                               ; preds = %8
  store i64 0, ptr %3, align 8
  %26 = load i64, ptr %7, align 8
  store i64 %26, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define internal void @rl__hash_impl__int64_t_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  call void @rl_compute_hash__int64_t_r_int64_t(ptr %3, ptr %1)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_compute_hash_of__int64_t_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  call void @rl__hash_impl__int64_t_r_int64_t(ptr %3, ptr %1)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_compute_equal__int64_t_int64_t_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = load i64, ptr %1, align 8
  %6 = load i64, ptr %2, align 8
  %7 = icmp eq i64 %5, %6
  %8 = zext i1 %7 to i8
  store i8 %8, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define void @rl_compute_equal__double_double_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = load double, ptr %1, align 8
  %6 = load double, ptr %2, align 8
  %7 = fcmp oeq double %5, %6
  %8 = zext i1 %7 to i8
  store i8 %8, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define internal void @rl__equal_bytes__int8_t_8_int8_t_8_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  %6 = alloca i8, i64 1, align 1
  %7 = alloca i64, i64 1, align 8
  store i64 0, ptr %7, align 8
  store i64 0, ptr %7, align 8
  br label %8

8:                                                ; preds = %48, %3
  %9 = load i64, ptr %7, align 8
  %10 = icmp slt i64 %9, 8
  %11 = zext i1 %10 to i8
  %12 = icmp ne i8 %11, 0
  br i1 %12, label %13, label %52

13:                                               ; preds = %8
  %14 = load i64, ptr %7, align 8
  %15 = icmp sge i64 %14, 8
  %16 = zext i1 %15 to i8
  %17 = load i64, ptr %7, align 8
  %18 = icmp slt i64 %17, 0
  %19 = zext i1 %18 to i8
  %20 = or i8 %16, %19
  %21 = icmp eq i8 %20, 0
  %22 = zext i1 %21 to i8
  %23 = icmp ne i8 %22, 0
  br i1 %23, label %26, label %24

24:                                               ; preds = %13
  %25 = call i32 @puts(ptr @str_0)
  call void @llvm.trap()
  ret void

26:                                               ; preds = %13
  %27 = load i64, ptr %7, align 8
  %28 = getelementptr [8 x i8], ptr %1, i32 0, i64 %27
  %29 = load i64, ptr %7, align 8
  %30 = icmp sge i64 %29, 8
  %31 = zext i1 %30 to i8
  %32 = load i64, ptr %7, align 8
  %33 = icmp slt i64 %32, 0
  %34 = zext i1 %33 to i8
  %35 = or i8 %31, %34
  %36 = icmp eq i8 %35, 0
  %37 = zext i1 %36 to i8
  %38 = icmp ne i8 %37, 0
  br i1 %38, label %41, label %39

39:                                               ; preds = %26
  %40 = call i32 @puts(ptr @str_1)
  call void @llvm.trap()
  ret void

41:                                               ; preds = %26
  %42 = load i64, ptr %7, align 8
  %43 = getelementptr [8 x i8], ptr %2, i32 0, i64 %42
  call void @rl_compute_equal__int8_t_int8_t_r_bool(ptr %6, ptr %28, ptr %43)
  %44 = load i8, ptr %6, align 1
  %45 = icmp eq i8 %44, 0
  %46 = zext i1 %45 to i8
  %47 = icmp ne i8 %46, 0
  br i1 %47, label %51, label %48

48:                                               ; preds = %41
  %49 = load i64, ptr %7, align 8
  %50 = add i64 %49, 1
  store i64 %50, ptr %7, align 8
  br label %8

51:                                               ; preds = %41
  store i8 0, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void

52:                                               ; preds = %8
  store i8 1, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define void @rl_compute_equal__bool_bool_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = load i8, ptr %1, align 1
  %6 = load i8, ptr %2, align 1
  %7 = icmp eq i8 %5, %6
  %8 = zext i1 %7 to i8
  store i8 %8, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define void @rl_compute_equal__int8_t_int8_t_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = load i8, ptr %1, align 1
  %6 = load i8, ptr %2, align 1
  %7 = icmp eq i8 %5, %6
  %8 = zext i1 %7 to i8
  store i8 %8, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define internal void @rl__equal_impl__int64_t_int64_t_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  call void @rl_compute_equal__int64_t_int64_t_r_bool(ptr %4, ptr %1, ptr %2)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define void @rl_compute_equal_of__int64_t_int64_t_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  call void @rl__equal_impl__int64_t_int64_t_r_bool(ptr %4, ptr %1, ptr %2)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define internal void @rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i64, i64 1, align 8
  %5 = alloca i64, i64 1, align 8
  store i64 1, ptr %5, align 8
  br label %6

6:                                                ; preds = %12, %3
  %7 = load i64, ptr %5, align 8
  %8 = load i64, ptr %2, align 8
  %9 = icmp slt i64 %7, %8
  %10 = zext i1 %9 to i8
  %11 = icmp ne i8 %10, 0
  br i1 %11, label %12, label %15

12:                                               ; preds = %6
  %13 = load i64, ptr %5, align 8
  %14 = mul i64 %13, 2
  store i64 %14, ptr %5, align 8
  br label %6

15:                                               ; preds = %6
  store i64 0, ptr %4, align 8
  %16 = load i64, ptr %5, align 8
  store i64 %16, ptr %4, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 8, i1 false)
  ret void
}

define void @rl_m_drop__DictTint64_tTint64_tT(ptr %0) {
  %2 = alloca i64, i64 1, align 8
  store i64 0, ptr %2, align 8
  br label %3

3:                                                ; preds = %10, %1
  %4 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %5 = load i64, ptr %2, align 8
  %6 = load i64, ptr %4, align 8
  %7 = icmp slt i64 %5, %6
  %8 = zext i1 %7 to i8
  %9 = icmp ne i8 %8, 0
  br i1 %9, label %10, label %13

10:                                               ; preds = %3
  %11 = load i64, ptr %2, align 8
  %12 = add i64 %11, 1
  store i64 %12, ptr %2, align 8
  br label %3

13:                                               ; preds = %3
  %14 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %15 = load i64, ptr %14, align 8
  %16 = icmp ne i64 %15, 0
  %17 = zext i1 %16 to i8
  %18 = icmp ne i8 %17, 0
  br i1 %18, label %20, label %19

19:                                               ; preds = %13
  br label %23

20:                                               ; preds = %13
  %21 = getelementptr %Dict, ptr %0, i32 0, i32 0
  %22 = load ptr, ptr %21, align 8
  call void @free(ptr %22)
  br label %23

23:                                               ; preds = %20, %19
  %24 = getelementptr %Dict, ptr %0, i32 0, i32 1
  store i64 0, ptr %24, align 8
  %25 = getelementptr %Dict, ptr %0, i32 0, i32 2
  store i64 0, ptr %25, align 8
  ret void
}

define internal void @rl_m__grow__DictTint64_tTint64_tT(ptr %0) {
  %2 = alloca i64, i64 1, align 8
  %3 = alloca i64, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  %5 = alloca i64, i64 1, align 8
  %6 = alloca ptr, i64 1, align 8
  %7 = alloca i64, i64 1, align 8
  %8 = getelementptr %Dict, ptr %0, i32 0, i32 2
  store i64 0, ptr %7, align 8
  %9 = load i64, ptr %8, align 8
  store i64 %9, ptr %7, align 8
  %10 = getelementptr %Dict, ptr %0, i32 0, i32 0
  store ptr null, ptr %6, align 8
  %11 = load ptr, ptr %10, align 8
  store ptr %11, ptr %6, align 8
  %12 = getelementptr %Dict, ptr %0, i32 0, i32 1
  store i64 0, ptr %5, align 8
  %13 = load i64, ptr %12, align 8
  store i64 %13, ptr %5, align 8
  %14 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %15 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %16 = load i64, ptr %15, align 8
  %17 = add i64 %16, 1
  store i64 %17, ptr %4, align 8
  call void @rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr %3, ptr %0, ptr %4)
  %18 = load i64, ptr %3, align 8
  store i64 %18, ptr %14, align 8
  %19 = getelementptr %Dict, ptr %0, i32 0, i32 0
  %20 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %21 = load i64, ptr %20, align 8
  %22 = call { i64, i1 } @llvm.umul.with.overflow.i64(i64 %21, i64 32)
  %23 = extractvalue { i64, i1 } %22, 0
  %24 = call ptr @malloc(i64 %23)
  store ptr %24, ptr %19, align 8
  %25 = getelementptr %Dict, ptr %0, i32 0, i32 1
  store i64 0, ptr %25, align 8
  store i64 0, ptr %2, align 8
  br label %26

26:                                               ; preds = %33, %1
  %27 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %28 = load i64, ptr %2, align 8
  %29 = load i64, ptr %27, align 8
  %30 = icmp slt i64 %28, %29
  %31 = zext i1 %30 to i8
  %32 = icmp ne i8 %31, 0
  br i1 %32, label %33, label %41

33:                                               ; preds = %26
  %34 = getelementptr %Dict, ptr %0, i32 0, i32 0
  %35 = load i64, ptr %2, align 8
  %36 = load ptr, ptr %34, align 8
  %37 = getelementptr %Entry, ptr %36, i64 %35
  %38 = getelementptr %Entry, ptr %37, i32 0, i32 0
  store i8 0, ptr %38, align 1
  %39 = load i64, ptr %2, align 8
  %40 = add i64 %39, 1
  store i64 %40, ptr %2, align 8
  br label %26

41:                                               ; preds = %26
  store i64 0, ptr %2, align 8
  br label %42

42:                                               ; preds = %66, %41
  %43 = load i64, ptr %2, align 8
  %44 = load i64, ptr %7, align 8
  %45 = icmp slt i64 %43, %44
  %46 = zext i1 %45 to i8
  %47 = icmp ne i8 %46, 0
  br i1 %47, label %48, label %69

48:                                               ; preds = %42
  %49 = load i64, ptr %2, align 8
  %50 = load ptr, ptr %6, align 8
  %51 = getelementptr %Entry, ptr %50, i64 %49
  %52 = getelementptr %Entry, ptr %51, i32 0, i32 0
  %53 = load i8, ptr %52, align 1
  %54 = icmp ne i8 %53, 0
  br i1 %54, label %56, label %55

55:                                               ; preds = %48
  br label %66

56:                                               ; preds = %48
  %57 = getelementptr %Dict, ptr %0, i32 0, i32 0
  %58 = load i64, ptr %2, align 8
  %59 = load ptr, ptr %6, align 8
  %60 = getelementptr %Entry, ptr %59, i64 %58
  %61 = getelementptr %Entry, ptr %60, i32 0, i32 2
  %62 = load i64, ptr %2, align 8
  %63 = load ptr, ptr %6, align 8
  %64 = getelementptr %Entry, ptr %63, i64 %62
  %65 = getelementptr %Entry, ptr %64, i32 0, i32 3
  call void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr %0, ptr %57, ptr %61, ptr %65)
  br label %66

66:                                               ; preds = %56, %55
  %67 = load i64, ptr %2, align 8
  %68 = add i64 %67, 1
  store i64 %68, ptr %2, align 8
  br label %42

69:                                               ; preds = %42
  store i64 0, ptr %2, align 8
  br label %70

70:                                               ; preds = %76, %69
  %71 = load i64, ptr %2, align 8
  %72 = load i64, ptr %7, align 8
  %73 = icmp slt i64 %71, %72
  %74 = zext i1 %73 to i8
  %75 = icmp ne i8 %74, 0
  br i1 %75, label %76, label %79

76:                                               ; preds = %70
  %77 = load i64, ptr %2, align 8
  %78 = add i64 %77, 1
  store i64 %78, ptr %2, align 8
  br label %70

79:                                               ; preds = %70
  %80 = load ptr, ptr %6, align 8
  call void @free(ptr %80)
  ret void
}

define void @rl_m_clear__DictTint64_tTint64_tT(ptr %0) {
  %2 = alloca i64, i64 1, align 8
  %3 = alloca i64, i64 1, align 8
  store i64 0, ptr %3, align 8
  br label %4

4:                                                ; preds = %11, %1
  %5 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %6 = load i64, ptr %3, align 8
  %7 = load i64, ptr %5, align 8
  %8 = icmp slt i64 %6, %7
  %9 = zext i1 %8 to i8
  %10 = icmp ne i8 %9, 0
  br i1 %10, label %11, label %14

11:                                               ; preds = %4
  %12 = load i64, ptr %3, align 8
  %13 = add i64 %12, 1
  store i64 %13, ptr %3, align 8
  br label %4

14:                                               ; preds = %4
  %15 = getelementptr %Dict, ptr %0, i32 0, i32 0
  %16 = load ptr, ptr %15, align 8
  call void @free(ptr %16)
  %17 = getelementptr %Dict, ptr %0, i32 0, i32 2
  store i64 4, ptr %17, align 8
  %18 = getelementptr %Dict, ptr %0, i32 0, i32 1
  store i64 0, ptr %18, align 8
  %19 = getelementptr %Dict, ptr %0, i32 0, i32 0
  %20 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %21 = load i64, ptr %20, align 8
  %22 = call { i64, i1 } @llvm.umul.with.overflow.i64(i64 %21, i64 32)
  %23 = extractvalue { i64, i1 } %22, 0
  %24 = call ptr @malloc(i64 %23)
  store ptr %24, ptr %19, align 8
  store i64 0, ptr %2, align 8
  br label %25

25:                                               ; preds = %32, %14
  %26 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %27 = load i64, ptr %2, align 8
  %28 = load i64, ptr %26, align 8
  %29 = icmp slt i64 %27, %28
  %30 = zext i1 %29 to i8
  %31 = icmp ne i8 %30, 0
  br i1 %31, label %32, label %40

32:                                               ; preds = %25
  %33 = getelementptr %Dict, ptr %0, i32 0, i32 0
  %34 = load i64, ptr %2, align 8
  %35 = load ptr, ptr %33, align 8
  %36 = getelementptr %Entry, ptr %35, i64 %34
  %37 = getelementptr %Entry, ptr %36, i32 0, i32 0
  store i8 0, ptr %37, align 1
  %38 = load i64, ptr %2, align 8
  %39 = add i64 %38, 1
  store i64 %39, ptr %2, align 8
  br label %25

40:                                               ; preds = %25
  ret void
}

define void @rl_m_values__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr %0, ptr %1) {
  %3 = alloca %Vector.1, i64 1, align 8
  %4 = alloca %Vector.1, i64 1, align 8
  %5 = alloca i64, i64 1, align 8
  %6 = alloca i64, i64 1, align 8
  %7 = alloca %Vector.1, i64 1, align 8
  call void @rl_m_init__VectorTint64_tT(ptr %7)
  store i64 0, ptr %6, align 8
  store i64 0, ptr %5, align 8
  br label %8

8:                                                ; preds = %32, %2
  %9 = getelementptr %Dict, ptr %1, i32 0, i32 1
  %10 = load i64, ptr %6, align 8
  %11 = load i64, ptr %9, align 8
  %12 = icmp slt i64 %10, %11
  %13 = zext i1 %12 to i8
  %14 = icmp ne i8 %13, 0
  br i1 %14, label %15, label %35

15:                                               ; preds = %8
  %16 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %17 = load i64, ptr %5, align 8
  %18 = load ptr, ptr %16, align 8
  %19 = getelementptr %Entry, ptr %18, i64 %17
  %20 = getelementptr %Entry, ptr %19, i32 0, i32 0
  %21 = load i8, ptr %20, align 1
  %22 = icmp ne i8 %21, 0
  br i1 %22, label %24, label %23

23:                                               ; preds = %15
  br label %32

24:                                               ; preds = %15
  %25 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %26 = load i64, ptr %5, align 8
  %27 = load ptr, ptr %25, align 8
  %28 = getelementptr %Entry, ptr %27, i64 %26
  %29 = getelementptr %Entry, ptr %28, i32 0, i32 3
  call void @rl_m_append__VectorTint64_tT_int64_t(ptr %7, ptr %29)
  %30 = load i64, ptr %6, align 8
  %31 = add i64 %30, 1
  store i64 %31, ptr %6, align 8
  br label %32

32:                                               ; preds = %24, %23
  %33 = load i64, ptr %5, align 8
  %34 = add i64 %33, 1
  store i64 %34, ptr %5, align 8
  br label %8

35:                                               ; preds = %8
  call void @rl_m_init__VectorTint64_tT(ptr %4)
  call void @rl_m_assign__VectorTint64_tT_VectorTint64_tT(ptr %4, ptr %7)
  call void @llvm.memcpy.p0.p0.i64(ptr %3, ptr %4, i64 24, i1 false)
  call void @rl_m_drop__VectorTint64_tT(ptr %7)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 24, i1 false)
  ret void
}

define void @rl_m_keys__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr %0, ptr %1) {
  %3 = alloca %Vector.1, i64 1, align 8
  %4 = alloca %Vector.1, i64 1, align 8
  %5 = alloca i64, i64 1, align 8
  %6 = alloca i64, i64 1, align 8
  %7 = alloca %Vector.1, i64 1, align 8
  call void @rl_m_init__VectorTint64_tT(ptr %7)
  store i64 0, ptr %6, align 8
  store i64 0, ptr %5, align 8
  br label %8

8:                                                ; preds = %32, %2
  %9 = getelementptr %Dict, ptr %1, i32 0, i32 1
  %10 = load i64, ptr %6, align 8
  %11 = load i64, ptr %9, align 8
  %12 = icmp slt i64 %10, %11
  %13 = zext i1 %12 to i8
  %14 = icmp ne i8 %13, 0
  br i1 %14, label %15, label %35

15:                                               ; preds = %8
  %16 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %17 = load i64, ptr %5, align 8
  %18 = load ptr, ptr %16, align 8
  %19 = getelementptr %Entry, ptr %18, i64 %17
  %20 = getelementptr %Entry, ptr %19, i32 0, i32 0
  %21 = load i8, ptr %20, align 1
  %22 = icmp ne i8 %21, 0
  br i1 %22, label %24, label %23

23:                                               ; preds = %15
  br label %32

24:                                               ; preds = %15
  %25 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %26 = load i64, ptr %5, align 8
  %27 = load ptr, ptr %25, align 8
  %28 = getelementptr %Entry, ptr %27, i64 %26
  %29 = getelementptr %Entry, ptr %28, i32 0, i32 2
  call void @rl_m_append__VectorTint64_tT_int64_t(ptr %7, ptr %29)
  %30 = load i64, ptr %6, align 8
  %31 = add i64 %30, 1
  store i64 %31, ptr %6, align 8
  br label %32

32:                                               ; preds = %24, %23
  %33 = load i64, ptr %5, align 8
  %34 = add i64 %33, 1
  store i64 %34, ptr %5, align 8
  br label %8

35:                                               ; preds = %8
  call void @rl_m_init__VectorTint64_tT(ptr %4)
  call void @rl_m_assign__VectorTint64_tT_VectorTint64_tT(ptr %4, ptr %7)
  call void @llvm.memcpy.p0.p0.i64(ptr %3, ptr %4, i64 24, i1 false)
  call void @rl_m_drop__VectorTint64_tT(ptr %7)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 24, i1 false)
  ret void
}

define void @rl_m_remove__DictTint64_tTint64_tT_int64_t_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  %6 = alloca i8, i64 1, align 1
  %7 = alloca %Entry, i64 1, align 8
  %8 = alloca i64, i64 1, align 8
  %9 = alloca i64, i64 1, align 8
  %10 = alloca i8, i64 1, align 1
  %11 = alloca i8, i64 1, align 1
  %12 = alloca i64, i64 1, align 8
  %13 = alloca i64, i64 1, align 8
  %14 = alloca i64, i64 1, align 8
  %15 = alloca i64, i64 1, align 8
  call void @rl_compute_hash_of__int64_t_r_int64_t(ptr %15, ptr %2)
  %16 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %17 = load i64, ptr %15, align 8
  %18 = load i64, ptr %16, align 8
  %19 = srem i64 %17, %18
  store i64 %19, ptr %14, align 8
  store i64 0, ptr %13, align 8
  store i64 0, ptr %12, align 8
  br label %20

20:                                               ; preds = %72, %3
  br i1 true, label %21, label %149

21:                                               ; preds = %20
  %22 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %23 = load i64, ptr %12, align 8
  %24 = load i64, ptr %22, align 8
  %25 = icmp sge i64 %23, %24
  %26 = zext i1 %25 to i8
  %27 = icmp ne i8 %26, 0
  br i1 %27, label %145, label %28

28:                                               ; preds = %21
  %29 = load i64, ptr %12, align 8
  %30 = add i64 %29, 1
  store i64 %30, ptr %12, align 8
  %31 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %32 = load i64, ptr %14, align 8
  %33 = load ptr, ptr %31, align 8
  %34 = getelementptr %Entry, ptr %33, i64 %32
  %35 = getelementptr %Entry, ptr %34, i32 0, i32 0
  %36 = load i8, ptr %35, align 1
  %37 = icmp eq i8 %36, 0
  %38 = zext i1 %37 to i8
  %39 = icmp ne i8 %38, 0
  br i1 %39, label %144, label %40

40:                                               ; preds = %28
  %41 = getelementptr %Entry, ptr %34, i32 0, i32 1
  %42 = load i64, ptr %41, align 8
  %43 = load i64, ptr %15, align 8
  %44 = icmp eq i64 %42, %43
  %45 = zext i1 %44 to i8
  store i8 %45, ptr %11, align 1
  %46 = load i8, ptr %11, align 1
  %47 = icmp ne i8 %46, 0
  br i1 %47, label %48, label %51

48:                                               ; preds = %40
  %49 = getelementptr %Entry, ptr %34, i32 0, i32 2
  call void @rl_compute_equal_of__int64_t_int64_t_r_bool(ptr %10, ptr %49, ptr %2)
  %50 = load i8, ptr %10, align 1
  store i8 %50, ptr %11, align 1
  br label %51

51:                                               ; preds = %48, %40
  %52 = load i8, ptr %11, align 1
  %53 = icmp ne i8 %52, 0
  br i1 %53, label %81, label %54

54:                                               ; preds = %51
  %55 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %56 = load i64, ptr %14, align 8
  %57 = load i64, ptr %55, align 8
  %58 = add i64 %56, %57
  %59 = getelementptr %Entry, ptr %34, i32 0, i32 1
  %60 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %61 = load i64, ptr %59, align 8
  %62 = load i64, ptr %60, align 8
  %63 = srem i64 %61, %62
  %64 = sub i64 %58, %63
  %65 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %66 = load i64, ptr %65, align 8
  %67 = srem i64 %64, %66
  %68 = load i64, ptr %13, align 8
  %69 = icmp slt i64 %67, %68
  %70 = zext i1 %69 to i8
  %71 = icmp ne i8 %70, 0
  br i1 %71, label %80, label %72

72:                                               ; preds = %54
  %73 = load i64, ptr %13, align 8
  %74 = add i64 %73, 1
  store i64 %74, ptr %13, align 8
  %75 = load i64, ptr %14, align 8
  %76 = add i64 %75, 1
  %77 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %78 = load i64, ptr %77, align 8
  %79 = srem i64 %76, %78
  store i64 %79, ptr %14, align 8
  br label %20

80:                                               ; preds = %54
  br label %149

81:                                               ; preds = %51
  %82 = getelementptr %Dict, ptr %1, i32 0, i32 1
  %83 = getelementptr %Dict, ptr %1, i32 0, i32 1
  %84 = load i64, ptr %83, align 8
  %85 = sub i64 %84, 1
  store i64 %85, ptr %82, align 8
  %86 = load i64, ptr %14, align 8
  %87 = add i64 %86, 1
  %88 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %89 = load i64, ptr %88, align 8
  %90 = srem i64 %87, %89
  store i64 %90, ptr %9, align 8
  store i64 0, ptr %8, align 8
  %91 = load i64, ptr %14, align 8
  store i64 %91, ptr %8, align 8
  br label %92

92:                                               ; preds = %120, %81
  br i1 true, label %93, label %143

93:                                               ; preds = %92
  %94 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %95 = load i64, ptr %9, align 8
  %96 = load ptr, ptr %94, align 8
  %97 = getelementptr %Entry, ptr %96, i64 %95
  call void @rl_m_init__EntryTint64_tTint64_tT(ptr %7)
  call void @rl_m_assign__EntryTint64_tTint64_tT_EntryTint64_tTint64_tT(ptr %7, ptr %97)
  %98 = getelementptr %Entry, ptr %7, i32 0, i32 0
  %99 = load i8, ptr %98, align 1
  %100 = icmp eq i8 %99, 0
  %101 = zext i1 %100 to i8
  %102 = icmp ne i8 %101, 0
  br i1 %102, label %137, label %103

103:                                              ; preds = %93
  %104 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %105 = load i64, ptr %9, align 8
  %106 = load i64, ptr %104, align 8
  %107 = add i64 %105, %106
  %108 = getelementptr %Entry, ptr %7, i32 0, i32 1
  %109 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %110 = load i64, ptr %108, align 8
  %111 = load i64, ptr %109, align 8
  %112 = srem i64 %110, %111
  %113 = sub i64 %107, %112
  %114 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %115 = load i64, ptr %114, align 8
  %116 = srem i64 %113, %115
  %117 = icmp eq i64 %116, 0
  %118 = zext i1 %117 to i8
  %119 = icmp ne i8 %118, 0
  br i1 %119, label %131, label %120

120:                                              ; preds = %103
  %121 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %122 = load i64, ptr %8, align 8
  %123 = load ptr, ptr %121, align 8
  %124 = getelementptr %Entry, ptr %123, i64 %122
  call void @rl_m_assign__EntryTint64_tTint64_tT_EntryTint64_tTint64_tT(ptr %124, ptr %7)
  %125 = load i64, ptr %9, align 8
  store i64 %125, ptr %8, align 8
  %126 = load i64, ptr %9, align 8
  %127 = add i64 %126, 1
  %128 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %129 = load i64, ptr %128, align 8
  %130 = srem i64 %127, %129
  store i64 %130, ptr %9, align 8
  br label %92

131:                                              ; preds = %103
  %132 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %133 = load i64, ptr %8, align 8
  %134 = load ptr, ptr %132, align 8
  %135 = getelementptr %Entry, ptr %134, i64 %133
  %136 = getelementptr %Entry, ptr %135, i32 0, i32 0
  store i8 0, ptr %136, align 1
  br label %143

137:                                              ; preds = %93
  %138 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %139 = load i64, ptr %8, align 8
  %140 = load ptr, ptr %138, align 8
  %141 = getelementptr %Entry, ptr %140, i64 %139
  %142 = getelementptr %Entry, ptr %141, i32 0, i32 0
  store i8 0, ptr %142, align 1
  br label %143

143:                                              ; preds = %137, %131, %92
  store i8 1, ptr %6, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %6, i64 1, i1 false)
  ret void

144:                                              ; preds = %28
  br label %149

145:                                              ; preds = %21
  br i1 false, label %148, label %146

146:                                              ; preds = %145
  %147 = call i32 @puts(ptr @str_2)
  call void @llvm.trap()
  ret void

148:                                              ; preds = %145
  store i8 0, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void

149:                                              ; preds = %144, %80, %20
  store i8 0, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define void @rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  %6 = alloca i8, i64 1, align 1
  %7 = alloca i8, i64 1, align 1
  %8 = alloca i8, i64 1, align 1
  %9 = alloca i64, i64 1, align 8
  %10 = alloca i64, i64 1, align 8
  %11 = alloca i64, i64 1, align 8
  %12 = alloca i64, i64 1, align 8
  %13 = getelementptr %Dict, ptr %1, i32 0, i32 1
  %14 = load i64, ptr %13, align 8
  %15 = icmp eq i64 %14, 0
  %16 = zext i1 %15 to i8
  %17 = icmp ne i8 %16, 0
  br i1 %17, label %93, label %18

18:                                               ; preds = %3
  call void @rl_compute_hash_of__int64_t_r_int64_t(ptr %12, ptr %2)
  %19 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %20 = load i64, ptr %12, align 8
  %21 = load i64, ptr %19, align 8
  %22 = srem i64 %20, %21
  store i64 %22, ptr %11, align 8
  store i64 0, ptr %10, align 8
  store i64 0, ptr %9, align 8
  store i8 0, ptr %8, align 1
  store i8 0, ptr %8, align 1
  br label %23

23:                                               ; preds = %80, %18
  br i1 true, label %24, label %91

24:                                               ; preds = %23
  %25 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %26 = load i64, ptr %9, align 8
  %27 = load i64, ptr %25, align 8
  %28 = icmp sge i64 %26, %27
  %29 = zext i1 %28 to i8
  %30 = icmp ne i8 %29, 0
  br i1 %30, label %32, label %31

31:                                               ; preds = %24
  br label %36

32:                                               ; preds = %24
  br i1 false, label %35, label %33

33:                                               ; preds = %32
  %34 = call i32 @puts(ptr @str_3)
  call void @llvm.trap()
  ret void

35:                                               ; preds = %32
  br label %36

36:                                               ; preds = %35, %31
  %37 = load i64, ptr %9, align 8
  %38 = add i64 %37, 1
  store i64 %38, ptr %9, align 8
  %39 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %40 = load i64, ptr %11, align 8
  %41 = load ptr, ptr %39, align 8
  %42 = getelementptr %Entry, ptr %41, i64 %40
  %43 = getelementptr %Entry, ptr %42, i32 0, i32 0
  %44 = load i8, ptr %43, align 1
  %45 = icmp eq i8 %44, 0
  %46 = zext i1 %45 to i8
  %47 = icmp ne i8 %46, 0
  br i1 %47, label %90, label %48

48:                                               ; preds = %36
  %49 = getelementptr %Entry, ptr %42, i32 0, i32 1
  %50 = load i64, ptr %49, align 8
  %51 = load i64, ptr %12, align 8
  %52 = icmp eq i64 %50, %51
  %53 = zext i1 %52 to i8
  store i8 %53, ptr %7, align 1
  %54 = load i8, ptr %7, align 1
  %55 = icmp ne i8 %54, 0
  br i1 %55, label %56, label %59

56:                                               ; preds = %48
  %57 = getelementptr %Entry, ptr %42, i32 0, i32 2
  call void @rl_compute_equal_of__int64_t_int64_t_r_bool(ptr %6, ptr %57, ptr %2)
  %58 = load i8, ptr %6, align 1
  store i8 %58, ptr %7, align 1
  br label %59

59:                                               ; preds = %56, %48
  %60 = load i8, ptr %7, align 1
  %61 = icmp ne i8 %60, 0
  br i1 %61, label %89, label %62

62:                                               ; preds = %59
  %63 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %64 = load i64, ptr %11, align 8
  %65 = load i64, ptr %63, align 8
  %66 = add i64 %64, %65
  %67 = getelementptr %Entry, ptr %42, i32 0, i32 1
  %68 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %69 = load i64, ptr %67, align 8
  %70 = load i64, ptr %68, align 8
  %71 = srem i64 %69, %70
  %72 = sub i64 %66, %71
  %73 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %74 = load i64, ptr %73, align 8
  %75 = srem i64 %72, %74
  %76 = load i64, ptr %10, align 8
  %77 = icmp slt i64 %75, %76
  %78 = zext i1 %77 to i8
  %79 = icmp ne i8 %78, 0
  br i1 %79, label %88, label %80

80:                                               ; preds = %62
  %81 = load i64, ptr %10, align 8
  %82 = add i64 %81, 1
  store i64 %82, ptr %10, align 8
  %83 = load i64, ptr %11, align 8
  %84 = add i64 %83, 1
  %85 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %86 = load i64, ptr %85, align 8
  %87 = srem i64 %84, %86
  store i64 %87, ptr %11, align 8
  br label %23

88:                                               ; preds = %62
  br label %91

89:                                               ; preds = %59
  store i8 1, ptr %8, align 1
  br label %91

90:                                               ; preds = %36
  br label %91

91:                                               ; preds = %90, %89, %88, %23
  store i8 0, ptr %5, align 1
  %92 = load i8, ptr %8, align 1
  store i8 %92, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void

93:                                               ; preds = %3
  store i8 0, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i64, i64 1, align 8
  %5 = alloca i64, i64 1, align 8
  %6 = alloca i8, i64 1, align 1
  %7 = alloca i8, i64 1, align 1
  %8 = alloca i64, i64 1, align 8
  %9 = alloca i64, i64 1, align 8
  %10 = alloca i64, i64 1, align 8
  %11 = alloca i64, i64 1, align 8
  %12 = getelementptr %Dict, ptr %1, i32 0, i32 1
  %13 = load i64, ptr %12, align 8
  %14 = icmp eq i64 %13, 0
  %15 = zext i1 %14 to i8
  %16 = icmp ne i8 %15, 0
  br i1 %16, label %18, label %17

17:                                               ; preds = %3
  br label %22

18:                                               ; preds = %3
  br i1 false, label %21, label %19

19:                                               ; preds = %18
  %20 = call i32 @puts(ptr @str_4)
  call void @llvm.trap()
  ret void

21:                                               ; preds = %18
  br label %22

22:                                               ; preds = %21, %17
  call void @rl_compute_hash_of__int64_t_r_int64_t(ptr %11, ptr %2)
  %23 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %24 = load i64, ptr %11, align 8
  %25 = load i64, ptr %23, align 8
  %26 = srem i64 %24, %25
  store i64 %26, ptr %10, align 8
  store i64 0, ptr %9, align 8
  store i64 0, ptr %8, align 8
  br label %27

27:                                               ; preds = %104, %22
  br i1 true, label %28, label %105

28:                                               ; preds = %27
  %29 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %30 = load i64, ptr %8, align 8
  %31 = load i64, ptr %29, align 8
  %32 = icmp sge i64 %30, %31
  %33 = zext i1 %32 to i8
  %34 = icmp ne i8 %33, 0
  br i1 %34, label %36, label %35

35:                                               ; preds = %28
  br label %40

36:                                               ; preds = %28
  br i1 false, label %39, label %37

37:                                               ; preds = %36
  %38 = call i32 @puts(ptr @str_5)
  call void @llvm.trap()
  ret void

39:                                               ; preds = %36
  br label %40

40:                                               ; preds = %39, %35
  %41 = load i64, ptr %8, align 8
  %42 = add i64 %41, 1
  store i64 %42, ptr %8, align 8
  %43 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %44 = load i64, ptr %10, align 8
  %45 = load ptr, ptr %43, align 8
  %46 = getelementptr %Entry, ptr %45, i64 %44
  %47 = getelementptr %Entry, ptr %46, i32 0, i32 0
  %48 = load i8, ptr %47, align 1
  %49 = icmp eq i8 %48, 0
  %50 = zext i1 %49 to i8
  %51 = icmp ne i8 %50, 0
  br i1 %51, label %100, label %52

52:                                               ; preds = %40
  %53 = getelementptr %Entry, ptr %46, i32 0, i32 1
  %54 = load i64, ptr %53, align 8
  %55 = load i64, ptr %11, align 8
  %56 = icmp eq i64 %54, %55
  %57 = zext i1 %56 to i8
  store i8 %57, ptr %7, align 1
  %58 = load i8, ptr %7, align 1
  %59 = icmp ne i8 %58, 0
  br i1 %59, label %60, label %63

60:                                               ; preds = %52
  %61 = getelementptr %Entry, ptr %46, i32 0, i32 2
  call void @rl_compute_equal_of__int64_t_int64_t_r_bool(ptr %6, ptr %61, ptr %2)
  %62 = load i8, ptr %6, align 1
  store i8 %62, ptr %7, align 1
  br label %63

63:                                               ; preds = %60, %52
  %64 = load i8, ptr %7, align 1
  %65 = icmp ne i8 %64, 0
  br i1 %65, label %97, label %66

66:                                               ; preds = %63
  %67 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %68 = load i64, ptr %10, align 8
  %69 = load i64, ptr %67, align 8
  %70 = add i64 %68, %69
  %71 = getelementptr %Entry, ptr %46, i32 0, i32 1
  %72 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %73 = load i64, ptr %71, align 8
  %74 = load i64, ptr %72, align 8
  %75 = srem i64 %73, %74
  %76 = sub i64 %70, %75
  %77 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %78 = load i64, ptr %77, align 8
  %79 = srem i64 %76, %78
  %80 = load i64, ptr %9, align 8
  %81 = icmp slt i64 %79, %80
  %82 = zext i1 %81 to i8
  %83 = icmp ne i8 %82, 0
  br i1 %83, label %85, label %84

84:                                               ; preds = %66
  br label %89

85:                                               ; preds = %66
  br i1 false, label %88, label %86

86:                                               ; preds = %85
  %87 = call i32 @puts(ptr @str_6)
  call void @llvm.trap()
  ret void

88:                                               ; preds = %85
  br label %89

89:                                               ; preds = %88, %84
  %90 = load i64, ptr %9, align 8
  %91 = add i64 %90, 1
  store i64 %91, ptr %9, align 8
  %92 = load i64, ptr %10, align 8
  %93 = add i64 %92, 1
  %94 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %95 = load i64, ptr %94, align 8
  %96 = srem i64 %93, %95
  store i64 %96, ptr %10, align 8
  br label %104

97:                                               ; preds = %63
  %98 = getelementptr %Entry, ptr %46, i32 0, i32 3
  store i64 0, ptr %5, align 8
  %99 = load i64, ptr %98, align 8
  store i64 %99, ptr %5, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 8, i1 false)
  ret void

100:                                              ; preds = %40
  br i1 false, label %103, label %101

101:                                              ; preds = %100
  %102 = call i32 @puts(ptr @str_7)
  call void @llvm.trap()
  ret void

103:                                              ; preds = %100
  br label %104

104:                                              ; preds = %103, %89
  br label %27

105:                                              ; preds = %27
  %106 = getelementptr %Dict, ptr %1, i32 0, i32 0
  %107 = load i64, ptr %10, align 8
  %108 = load ptr, ptr %106, align 8
  %109 = getelementptr %Entry, ptr %108, i64 %107
  %110 = getelementptr %Entry, ptr %109, i32 0, i32 3
  store i64 0, ptr %4, align 8
  %111 = load i64, ptr %110, align 8
  store i64 %111, ptr %4, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 8, i1 false)
  ret void
}

define internal void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr %0, ptr %1, ptr %2, ptr %3) {
  %5 = alloca %Entry, i64 1, align 8
  %6 = alloca %Entry, i64 1, align 8
  %7 = alloca i64, i64 1, align 8
  %8 = alloca i8, i64 1, align 1
  %9 = alloca i8, i64 1, align 1
  %10 = alloca i64, i64 1, align 8
  %11 = alloca i64, i64 1, align 8
  %12 = alloca i64, i64 1, align 8
  %13 = alloca i64, i64 1, align 8
  %14 = alloca i64, i64 1, align 8
  %15 = alloca i64, i64 1, align 8
  %16 = alloca i64, i64 1, align 8
  call void @rl_compute_hash_of__int64_t_r_int64_t(ptr %16, ptr %2)
  %17 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %18 = load i64, ptr %16, align 8
  %19 = load i64, ptr %17, align 8
  %20 = srem i64 %18, %19
  store i64 %20, ptr %15, align 8
  store i64 0, ptr %14, align 8
  store i64 0, ptr %13, align 8
  store i64 0, ptr %12, align 8
  %21 = load i64, ptr %2, align 8
  store i64 %21, ptr %12, align 8
  store i64 0, ptr %11, align 8
  %22 = load i64, ptr %3, align 8
  store i64 %22, ptr %11, align 8
  store i64 0, ptr %10, align 8
  %23 = load i64, ptr %16, align 8
  store i64 %23, ptr %10, align 8
  br label %24

24:                                               ; preds = %103, %4
  br i1 true, label %25, label %145

25:                                               ; preds = %24
  %26 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %27 = load i64, ptr %13, align 8
  %28 = load i64, ptr %26, align 8
  %29 = icmp sge i64 %27, %28
  %30 = zext i1 %29 to i8
  %31 = icmp ne i8 %30, 0
  br i1 %31, label %141, label %32

32:                                               ; preds = %25
  %33 = load i64, ptr %13, align 8
  %34 = add i64 %33, 1
  store i64 %34, ptr %13, align 8
  %35 = load i64, ptr %15, align 8
  %36 = load ptr, ptr %1, align 8
  %37 = getelementptr %Entry, ptr %36, i64 %35
  %38 = getelementptr %Entry, ptr %37, i32 0, i32 0
  %39 = load i8, ptr %38, align 1
  %40 = icmp eq i8 %39, 0
  %41 = zext i1 %40 to i8
  %42 = icmp ne i8 %41, 0
  br i1 %42, label %120, label %43

43:                                               ; preds = %32
  %44 = load i64, ptr %15, align 8
  %45 = load ptr, ptr %1, align 8
  %46 = getelementptr %Entry, ptr %45, i64 %44
  %47 = getelementptr %Entry, ptr %46, i32 0, i32 1
  %48 = load i64, ptr %47, align 8
  %49 = load i64, ptr %10, align 8
  %50 = icmp eq i64 %48, %49
  %51 = zext i1 %50 to i8
  store i8 %51, ptr %9, align 1
  %52 = load i8, ptr %9, align 1
  %53 = icmp ne i8 %52, 0
  br i1 %53, label %54, label %60

54:                                               ; preds = %43
  %55 = load i64, ptr %15, align 8
  %56 = load ptr, ptr %1, align 8
  %57 = getelementptr %Entry, ptr %56, i64 %55
  %58 = getelementptr %Entry, ptr %57, i32 0, i32 2
  call void @rl_compute_equal_of__int64_t_int64_t_r_bool(ptr %8, ptr %58, ptr %12)
  %59 = load i8, ptr %8, align 1
  store i8 %59, ptr %9, align 1
  br label %60

60:                                               ; preds = %54, %43
  %61 = load i8, ptr %9, align 1
  %62 = icmp ne i8 %61, 0
  br i1 %62, label %111, label %63

63:                                               ; preds = %60
  %64 = load i64, ptr %15, align 8
  %65 = load ptr, ptr %1, align 8
  %66 = getelementptr %Entry, ptr %65, i64 %64
  %67 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %68 = load i64, ptr %15, align 8
  %69 = load i64, ptr %67, align 8
  %70 = add i64 %68, %69
  %71 = getelementptr %Entry, ptr %66, i32 0, i32 1
  %72 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %73 = load i64, ptr %71, align 8
  %74 = load i64, ptr %72, align 8
  %75 = srem i64 %73, %74
  %76 = sub i64 %70, %75
  %77 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %78 = load i64, ptr %77, align 8
  %79 = srem i64 %76, %78
  store i64 %79, ptr %7, align 8
  %80 = load i64, ptr %7, align 8
  %81 = load i64, ptr %14, align 8
  %82 = icmp slt i64 %80, %81
  %83 = zext i1 %82 to i8
  %84 = icmp ne i8 %83, 0
  br i1 %84, label %86, label %85

85:                                               ; preds = %63
  br label %103

86:                                               ; preds = %63
  call void @rl_m_init__EntryTint64_tTint64_tT(ptr %6)
  call void @rl_m_assign__EntryTint64_tTint64_tT_EntryTint64_tTint64_tT(ptr %6, ptr %66)
  %87 = getelementptr %Entry, ptr %66, i32 0, i32 1
  %88 = load i64, ptr %10, align 8
  store i64 %88, ptr %87, align 8
  %89 = getelementptr %Entry, ptr %66, i32 0, i32 2
  %90 = load i64, ptr %12, align 8
  store i64 %90, ptr %89, align 8
  %91 = getelementptr %Entry, ptr %66, i32 0, i32 3
  %92 = load i64, ptr %11, align 8
  store i64 %92, ptr %91, align 8
  %93 = load i64, ptr %15, align 8
  %94 = load ptr, ptr %1, align 8
  %95 = getelementptr %Entry, ptr %94, i64 %93
  call void @rl_m_assign__EntryTint64_tTint64_tT_EntryTint64_tTint64_tT(ptr %95, ptr %66)
  %96 = getelementptr %Entry, ptr %6, i32 0, i32 1
  %97 = load i64, ptr %96, align 8
  store i64 %97, ptr %10, align 8
  %98 = getelementptr %Entry, ptr %6, i32 0, i32 2
  %99 = load i64, ptr %98, align 8
  store i64 %99, ptr %12, align 8
  %100 = getelementptr %Entry, ptr %6, i32 0, i32 3
  %101 = load i64, ptr %100, align 8
  store i64 %101, ptr %11, align 8
  %102 = load i64, ptr %7, align 8
  store i64 %102, ptr %14, align 8
  br label %103

103:                                              ; preds = %86, %85
  %104 = load i64, ptr %14, align 8
  %105 = add i64 %104, 1
  store i64 %105, ptr %14, align 8
  %106 = load i64, ptr %15, align 8
  %107 = add i64 %106, 1
  %108 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %109 = load i64, ptr %108, align 8
  %110 = srem i64 %107, %109
  store i64 %110, ptr %15, align 8
  br label %24

111:                                              ; preds = %60
  %112 = load i64, ptr %15, align 8
  %113 = load ptr, ptr %1, align 8
  %114 = getelementptr %Entry, ptr %113, i64 %112
  %115 = getelementptr %Entry, ptr %114, i32 0, i32 3
  %116 = load i64, ptr %11, align 8
  store i64 %116, ptr %115, align 8
  %117 = load i64, ptr %15, align 8
  %118 = load ptr, ptr %1, align 8
  %119 = getelementptr %Entry, ptr %118, i64 %117
  call void @rl_m_assign__EntryTint64_tTint64_tT_EntryTint64_tTint64_tT(ptr %119, ptr %114)
  ret void

120:                                              ; preds = %32
  %121 = load i64, ptr %15, align 8
  %122 = load ptr, ptr %1, align 8
  %123 = getelementptr %Entry, ptr %122, i64 %121
  call void @rl_m_init__EntryTint64_tTint64_tT(ptr %123)
  %124 = load i64, ptr %15, align 8
  %125 = load ptr, ptr %1, align 8
  %126 = getelementptr %Entry, ptr %125, i64 %124
  call void @rl_m_init__EntryTint64_tTint64_tT(ptr %5)
  call void @rl_m_assign__EntryTint64_tTint64_tT_EntryTint64_tTint64_tT(ptr %5, ptr %126)
  %127 = getelementptr %Entry, ptr %5, i32 0, i32 0
  store i8 1, ptr %127, align 1
  %128 = getelementptr %Entry, ptr %5, i32 0, i32 1
  %129 = load i64, ptr %10, align 8
  store i64 %129, ptr %128, align 8
  %130 = getelementptr %Entry, ptr %5, i32 0, i32 2
  %131 = load i64, ptr %12, align 8
  store i64 %131, ptr %130, align 8
  %132 = getelementptr %Entry, ptr %5, i32 0, i32 3
  %133 = load i64, ptr %11, align 8
  store i64 %133, ptr %132, align 8
  %134 = load i64, ptr %15, align 8
  %135 = load ptr, ptr %1, align 8
  %136 = getelementptr %Entry, ptr %135, i64 %134
  call void @rl_m_assign__EntryTint64_tTint64_tT_EntryTint64_tTint64_tT(ptr %136, ptr %5)
  %137 = getelementptr %Dict, ptr %0, i32 0, i32 1
  %138 = getelementptr %Dict, ptr %0, i32 0, i32 1
  %139 = load i64, ptr %138, align 8
  %140 = add i64 %139, 1
  store i64 %140, ptr %137, align 8
  ret void

141:                                              ; preds = %25
  br i1 false, label %144, label %142

142:                                              ; preds = %141
  %143 = call i32 @puts(ptr @str_8)
  call void @llvm.trap()
  ret void

144:                                              ; preds = %141
  ret void

145:                                              ; preds = %24
  ret void
}

define void @rl_m_insert__DictTint64_tTint64_tT_int64_t_int64_t_r_bool(ptr %0, ptr %1, ptr %2, ptr %3) {
  %5 = alloca i8, i64 1, align 1
  %6 = alloca double, i64 1, align 8
  store double 0.000000e+00, ptr %6, align 8
  %7 = getelementptr %Dict, ptr %1, i32 0, i32 1
  %8 = load i64, ptr %7, align 8
  %9 = add i64 %8, 1
  %10 = sitofp i64 %9 to double
  %11 = getelementptr %Dict, ptr %1, i32 0, i32 2
  %12 = load i64, ptr %11, align 8
  %13 = sitofp i64 %12 to double
  %14 = fdiv double %10, %13
  store double %14, ptr %6, align 8
  %15 = getelementptr %Dict, ptr %1, i32 0, i32 3
  %16 = load double, ptr %6, align 8
  %17 = load double, ptr %15, align 8
  %18 = fcmp ogt double %16, %17
  %19 = zext i1 %18 to i8
  %20 = icmp ne i8 %19, 0
  br i1 %20, label %22, label %21

21:                                               ; preds = %4
  br label %23

22:                                               ; preds = %4
  call void @rl_m__grow__DictTint64_tTint64_tT(ptr %1)
  br label %23

23:                                               ; preds = %22, %21
  %24 = getelementptr %Dict, ptr %1, i32 0, i32 0
  call void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr %1, ptr %24, ptr %2, ptr %3)
  store i8 1, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void
}

define void @rl_m_init__DictTint64_tTint64_tT(ptr %0) {
  %2 = alloca i64, i64 1, align 8
  %3 = getelementptr %Dict, ptr %0, i32 0, i32 2
  store i64 4, ptr %3, align 8
  %4 = getelementptr %Dict, ptr %0, i32 0, i32 1
  store i64 0, ptr %4, align 8
  %5 = getelementptr %Dict, ptr %0, i32 0, i32 3
  store double 7.500000e-01, ptr %5, align 8
  %6 = getelementptr %Dict, ptr %0, i32 0, i32 0
  %7 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %8 = load i64, ptr %7, align 8
  %9 = call { i64, i1 } @llvm.umul.with.overflow.i64(i64 %8, i64 32)
  %10 = extractvalue { i64, i1 } %9, 0
  %11 = call ptr @malloc(i64 %10)
  store ptr %11, ptr %6, align 8
  store i64 0, ptr %2, align 8
  br label %12

12:                                               ; preds = %19, %1
  %13 = getelementptr %Dict, ptr %0, i32 0, i32 2
  %14 = load i64, ptr %2, align 8
  %15 = load i64, ptr %13, align 8
  %16 = icmp slt i64 %14, %15
  %17 = zext i1 %16 to i8
  %18 = icmp ne i8 %17, 0
  br i1 %18, label %19, label %27

19:                                               ; preds = %12
  %20 = getelementptr %Dict, ptr %0, i32 0, i32 0
  %21 = load i64, ptr %2, align 8
  %22 = load ptr, ptr %20, align 8
  %23 = getelementptr %Entry, ptr %22, i64 %21
  %24 = getelementptr %Entry, ptr %23, i32 0, i32 0
  store i8 0, ptr %24, align 1
  %25 = load i64, ptr %2, align 8
  %26 = add i64 %25, 1
  store i64 %26, ptr %2, align 8
  br label %12

27:                                               ; preds = %12
  ret void
}

define void @rl_none__r_Nothing(ptr %0) {
  %2 = alloca %Nothing, i64 1, align 8
  %3 = alloca %Nothing, i64 1, align 8
  call void @rl_m_init__Nothing(ptr %3)
  call void @rl_m_init__Nothing(ptr %2)
  call void @rl_m_assign__Nothing_Nothing(ptr %2, ptr %3)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %2, i64 1, i1 false)
  ret void
}

define void @rl_m_set_size__Range_int64_t(ptr %0, ptr %1) {
  %3 = getelementptr %Range, ptr %0, i32 0, i32 0
  %4 = load i64, ptr %1, align 8
  store i64 %4, ptr %3, align 8
  ret void
}

define void @rl_m_size__Range_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = getelementptr %Range, ptr %1, i32 0, i32 0
  store i64 0, ptr %3, align 8
  %5 = load i64, ptr %4, align 8
  store i64 %5, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_m_get__Range_int64_t_r_int64_t(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i64, i64 1, align 8
  store i64 0, ptr %4, align 8
  %5 = load i64, ptr %2, align 8
  store i64 %5, ptr %4, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 8, i1 false)
  ret void
}

define void @rl_range__int64_t_r_Range(ptr %0, ptr %1) {
  %3 = alloca %Range, i64 1, align 8
  %4 = alloca %Range, i64 1, align 8
  call void @rl_m_init__Range(ptr %4)
  call void @rl_m_set_size__Range_int64_t(ptr %4, ptr %1)
  call void @rl_m_init__Range(ptr %3)
  call void @rl_m_assign__Range_Range(ptr %3, ptr %4)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_m_size__VectorTint8_tT_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = getelementptr %Vector, ptr %1, i32 0, i32 1
  store i64 0, ptr %3, align 8
  %5 = load i64, ptr %4, align 8
  store i64 %5, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_m_size__VectorTint64_tT_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = getelementptr %Vector.1, ptr %1, i32 0, i32 1
  store i64 0, ptr %3, align 8
  %5 = load i64, ptr %4, align 8
  store i64 %5, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_m_drop_back__VectorTint8_tT_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = getelementptr %Vector, ptr %0, i32 0, i32 1
  %5 = load i64, ptr %4, align 8
  %6 = load i64, ptr %1, align 8
  %7 = sub i64 %5, %6
  store i64 %7, ptr %3, align 8
  br label %8

8:                                                ; preds = %15, %2
  %9 = getelementptr %Vector, ptr %0, i32 0, i32 1
  %10 = load i64, ptr %3, align 8
  %11 = load i64, ptr %9, align 8
  %12 = icmp slt i64 %10, %11
  %13 = zext i1 %12 to i8
  %14 = icmp ne i8 %13, 0
  br i1 %14, label %15, label %22

15:                                               ; preds = %8
  %16 = getelementptr %Vector, ptr %0, i32 0, i32 0
  %17 = load i64, ptr %3, align 8
  %18 = load ptr, ptr %16, align 8
  %19 = getelementptr i8, ptr %18, i64 %17
  store i8 0, ptr %19, align 1
  %20 = load i64, ptr %3, align 8
  %21 = add i64 %20, 1
  store i64 %21, ptr %3, align 8
  br label %8

22:                                               ; preds = %8
  %23 = getelementptr %Vector, ptr %0, i32 0, i32 1
  %24 = getelementptr %Vector, ptr %0, i32 0, i32 1
  %25 = load i64, ptr %24, align 8
  %26 = load i64, ptr %1, align 8
  %27 = sub i64 %25, %26
  store i64 %27, ptr %23, align 8
  ret void
}

define void @rl_m_pop__VectorTint8_tT_r_int8_t(ptr %0, ptr %1) {
  %3 = alloca i8, i64 1, align 1
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  %6 = getelementptr %Vector, ptr %1, i32 0, i32 1
  %7 = load i64, ptr %6, align 8
  %8 = icmp sgt i64 %7, 0
  %9 = zext i1 %8 to i8
  %10 = icmp ne i8 %9, 0
  br i1 %10, label %13, label %11

11:                                               ; preds = %2
  %12 = call i32 @puts(ptr @str_9)
  call void @llvm.trap()
  ret void

13:                                               ; preds = %2
  %14 = getelementptr %Vector, ptr %1, i32 0, i32 0
  %15 = getelementptr %Vector, ptr %1, i32 0, i32 1
  %16 = load i64, ptr %15, align 8
  %17 = sub i64 %16, 1
  %18 = load ptr, ptr %14, align 8
  %19 = getelementptr i8, ptr %18, i64 %17
  store i8 0, ptr %5, align 1
  %20 = load i8, ptr %19, align 1
  store i8 %20, ptr %5, align 1
  %21 = getelementptr %Vector, ptr %1, i32 0, i32 1
  %22 = getelementptr %Vector, ptr %1, i32 0, i32 1
  %23 = load i64, ptr %22, align 8
  %24 = sub i64 %23, 1
  store i64 %24, ptr %21, align 8
  %25 = getelementptr %Vector, ptr %1, i32 0, i32 0
  %26 = getelementptr %Vector, ptr %1, i32 0, i32 1
  %27 = load i64, ptr %26, align 8
  %28 = load ptr, ptr %25, align 8
  %29 = getelementptr i8, ptr %28, i64 %27
  store i8 0, ptr %29, align 1
  store i8 0, ptr %4, align 1
  %30 = load i8, ptr %5, align 1
  store i8 %30, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %3, ptr %4, i64 1, i1 false)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 1, i1 false)
  ret void
}

define void @rl_m_append__VectorTint8_tT_int8_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = getelementptr %Vector, ptr %0, i32 0, i32 1
  %5 = load i64, ptr %4, align 8
  %6 = add i64 %5, 1
  store i64 %6, ptr %3, align 8
  call void @rl_m__grow__VectorTint8_tT_int64_t(ptr %0, ptr %3)
  %7 = getelementptr %Vector, ptr %0, i32 0, i32 0
  %8 = getelementptr %Vector, ptr %0, i32 0, i32 1
  %9 = load i64, ptr %8, align 8
  %10 = load ptr, ptr %7, align 8
  %11 = getelementptr i8, ptr %10, i64 %9
  %12 = load i8, ptr %1, align 1
  store i8 %12, ptr %11, align 1
  %13 = getelementptr %Vector, ptr %0, i32 0, i32 1
  %14 = getelementptr %Vector, ptr %0, i32 0, i32 1
  %15 = load i64, ptr %14, align 8
  %16 = add i64 %15, 1
  store i64 %16, ptr %13, align 8
  ret void
}

define void @rl_m_append__VectorTint64_tT_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = getelementptr %Vector.1, ptr %0, i32 0, i32 1
  %5 = load i64, ptr %4, align 8
  %6 = add i64 %5, 1
  store i64 %6, ptr %3, align 8
  call void @rl_m__grow__VectorTint64_tT_int64_t(ptr %0, ptr %3)
  %7 = getelementptr %Vector.1, ptr %0, i32 0, i32 0
  %8 = getelementptr %Vector.1, ptr %0, i32 0, i32 1
  %9 = load i64, ptr %8, align 8
  %10 = load ptr, ptr %7, align 8
  %11 = getelementptr i64, ptr %10, i64 %9
  %12 = load i64, ptr %1, align 8
  store i64 %12, ptr %11, align 8
  %13 = getelementptr %Vector.1, ptr %0, i32 0, i32 1
  %14 = getelementptr %Vector.1, ptr %0, i32 0, i32 1
  %15 = load i64, ptr %14, align 8
  %16 = add i64 %15, 1
  store i64 %16, ptr %13, align 8
  ret void
}

define void @rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef(ptr %0, ptr %1, ptr %2) {
  %4 = load i64, ptr %2, align 8
  %5 = icmp sge i64 %4, 0
  %6 = zext i1 %5 to i8
  %7 = icmp ne i8 %6, 0
  br i1 %7, label %10, label %8

8:                                                ; preds = %3
  %9 = call i32 @puts(ptr @str_10)
  call void @llvm.trap()
  ret void

10:                                               ; preds = %3
  %11 = getelementptr %Vector, ptr %1, i32 0, i32 1
  %12 = load i64, ptr %2, align 8
  %13 = load i64, ptr %11, align 8
  %14 = icmp slt i64 %12, %13
  %15 = zext i1 %14 to i8
  %16 = icmp ne i8 %15, 0
  br i1 %16, label %19, label %17

17:                                               ; preds = %10
  %18 = call i32 @puts(ptr @str_11)
  call void @llvm.trap()
  ret void

19:                                               ; preds = %10
  %20 = getelementptr %Vector, ptr %1, i32 0, i32 0
  %21 = load i64, ptr %2, align 8
  %22 = load ptr, ptr %20, align 8
  %23 = getelementptr i8, ptr %22, i64 %21
  store ptr %23, ptr %0, align 8
  ret void
}

define void @rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef(ptr %0, ptr %1, ptr %2) {
  %4 = load i64, ptr %2, align 8
  %5 = icmp sge i64 %4, 0
  %6 = zext i1 %5 to i8
  %7 = icmp ne i8 %6, 0
  br i1 %7, label %10, label %8

8:                                                ; preds = %3
  %9 = call i32 @puts(ptr @str_10)
  call void @llvm.trap()
  ret void

10:                                               ; preds = %3
  %11 = getelementptr %Vector.1, ptr %1, i32 0, i32 1
  %12 = load i64, ptr %2, align 8
  %13 = load i64, ptr %11, align 8
  %14 = icmp slt i64 %12, %13
  %15 = zext i1 %14 to i8
  %16 = icmp ne i8 %15, 0
  br i1 %16, label %19, label %17

17:                                               ; preds = %10
  %18 = call i32 @puts(ptr @str_11)
  call void @llvm.trap()
  ret void

19:                                               ; preds = %10
  %20 = getelementptr %Vector.1, ptr %1, i32 0, i32 0
  %21 = load i64, ptr %2, align 8
  %22 = load ptr, ptr %20, align 8
  %23 = getelementptr i64, ptr %22, i64 %21
  store ptr %23, ptr %0, align 8
  ret void
}

define void @rl_m_back__VectorTint8_tT_r_int8_tRef(ptr %0, ptr %1) {
  %3 = getelementptr %Vector, ptr %1, i32 0, i32 1
  %4 = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %4, 0
  %6 = zext i1 %5 to i8
  %7 = icmp ne i8 %6, 0
  br i1 %7, label %10, label %8

8:                                                ; preds = %2
  %9 = call i32 @puts(ptr @str_12)
  call void @llvm.trap()
  ret void

10:                                               ; preds = %2
  %11 = getelementptr %Vector, ptr %1, i32 0, i32 0
  %12 = getelementptr %Vector, ptr %1, i32 0, i32 1
  %13 = load i64, ptr %12, align 8
  %14 = sub i64 %13, 1
  %15 = load ptr, ptr %11, align 8
  %16 = getelementptr i8, ptr %15, i64 %14
  store ptr %16, ptr %0, align 8
  ret void
}

define void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr %0, ptr %1) {
  %3 = alloca ptr, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  call void @rl_m_drop__VectorTint8_tT(ptr %0)
  call void @rl_m_init__VectorTint8_tT(ptr %0)
  store i64 0, ptr %4, align 8
  br label %5

5:                                                ; preds = %12, %2
  %6 = getelementptr %Vector, ptr %1, i32 0, i32 1
  %7 = load i64, ptr %4, align 8
  %8 = load i64, ptr %6, align 8
  %9 = icmp slt i64 %7, %8
  %10 = zext i1 %9 to i8
  %11 = icmp ne i8 %10, 0
  br i1 %11, label %12, label %16

12:                                               ; preds = %5
  call void @rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef(ptr %3, ptr %1, ptr %4)
  %13 = load ptr, ptr %3, align 8
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %0, ptr %13)
  %14 = load i64, ptr %4, align 8
  %15 = add i64 %14, 1
  store i64 %15, ptr %4, align 8
  br label %5

16:                                               ; preds = %5
  ret void
}

define void @rl_m_assign__VectorTint64_tT_VectorTint64_tT(ptr %0, ptr %1) {
  %3 = alloca ptr, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  call void @rl_m_drop__VectorTint64_tT(ptr %0)
  call void @rl_m_init__VectorTint64_tT(ptr %0)
  store i64 0, ptr %4, align 8
  br label %5

5:                                                ; preds = %12, %2
  %6 = getelementptr %Vector.1, ptr %1, i32 0, i32 1
  %7 = load i64, ptr %4, align 8
  %8 = load i64, ptr %6, align 8
  %9 = icmp slt i64 %7, %8
  %10 = zext i1 %9 to i8
  %11 = icmp ne i8 %10, 0
  br i1 %11, label %12, label %16

12:                                               ; preds = %5
  call void @rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef(ptr %3, ptr %1, ptr %4)
  %13 = load ptr, ptr %3, align 8
  call void @rl_m_append__VectorTint64_tT_int64_t(ptr %0, ptr %13)
  %14 = load i64, ptr %4, align 8
  %15 = add i64 %14, 1
  store i64 %15, ptr %4, align 8
  br label %5

16:                                               ; preds = %5
  ret void
}

define void @rl_m_drop__VectorTint8_tT(ptr %0) {
  %2 = alloca i64, i64 1, align 8
  store i64 0, ptr %2, align 8
  br label %3

3:                                                ; preds = %10, %1
  %4 = getelementptr %Vector, ptr %0, i32 0, i32 2
  %5 = load i64, ptr %2, align 8
  %6 = load i64, ptr %4, align 8
  %7 = icmp ne i64 %5, %6
  %8 = zext i1 %7 to i8
  %9 = icmp ne i8 %8, 0
  br i1 %9, label %10, label %13

10:                                               ; preds = %3
  %11 = load i64, ptr %2, align 8
  %12 = add i64 %11, 1
  store i64 %12, ptr %2, align 8
  br label %3

13:                                               ; preds = %3
  %14 = getelementptr %Vector, ptr %0, i32 0, i32 2
  %15 = load i64, ptr %14, align 8
  %16 = icmp ne i64 %15, 0
  %17 = zext i1 %16 to i8
  %18 = icmp ne i8 %17, 0
  br i1 %18, label %20, label %19

19:                                               ; preds = %13
  br label %23

20:                                               ; preds = %13
  %21 = getelementptr %Vector, ptr %0, i32 0, i32 0
  %22 = load ptr, ptr %21, align 8
  call void @free(ptr %22)
  br label %23

23:                                               ; preds = %20, %19
  %24 = getelementptr %Vector, ptr %0, i32 0, i32 1
  store i64 0, ptr %24, align 8
  %25 = getelementptr %Vector, ptr %0, i32 0, i32 2
  store i64 0, ptr %25, align 8
  ret void
}

define void @rl_m_drop__VectorTint64_tT(ptr %0) {
  %2 = alloca i64, i64 1, align 8
  store i64 0, ptr %2, align 8
  br label %3

3:                                                ; preds = %10, %1
  %4 = getelementptr %Vector.1, ptr %0, i32 0, i32 2
  %5 = load i64, ptr %2, align 8
  %6 = load i64, ptr %4, align 8
  %7 = icmp ne i64 %5, %6
  %8 = zext i1 %7 to i8
  %9 = icmp ne i8 %8, 0
  br i1 %9, label %10, label %13

10:                                               ; preds = %3
  %11 = load i64, ptr %2, align 8
  %12 = add i64 %11, 1
  store i64 %12, ptr %2, align 8
  br label %3

13:                                               ; preds = %3
  %14 = getelementptr %Vector.1, ptr %0, i32 0, i32 2
  %15 = load i64, ptr %14, align 8
  %16 = icmp ne i64 %15, 0
  %17 = zext i1 %16 to i8
  %18 = icmp ne i8 %17, 0
  br i1 %18, label %20, label %19

19:                                               ; preds = %13
  br label %23

20:                                               ; preds = %13
  %21 = getelementptr %Vector.1, ptr %0, i32 0, i32 0
  %22 = load ptr, ptr %21, align 8
  call void @free(ptr %22)
  br label %23

23:                                               ; preds = %20, %19
  %24 = getelementptr %Vector.1, ptr %0, i32 0, i32 1
  store i64 0, ptr %24, align 8
  %25 = getelementptr %Vector.1, ptr %0, i32 0, i32 2
  store i64 0, ptr %25, align 8
  ret void
}

define void @rl_m_init__VectorTint8_tT(ptr %0) {
  %2 = alloca i64, i64 1, align 8
  %3 = getelementptr %Vector, ptr %0, i32 0, i32 1
  store i64 0, ptr %3, align 8
  %4 = getelementptr %Vector, ptr %0, i32 0, i32 2
  store i64 4, ptr %4, align 8
  %5 = getelementptr %Vector, ptr %0, i32 0, i32 0
  %6 = call { i64, i1 } @llvm.umul.with.overflow.i64(i64 4, i64 1)
  %7 = extractvalue { i64, i1 } %6, 0
  %8 = call ptr @malloc(i64 %7)
  store ptr %8, ptr %5, align 8
  store i64 0, ptr %2, align 8
  br label %9

9:                                                ; preds = %16, %1
  %10 = getelementptr %Vector, ptr %0, i32 0, i32 2
  %11 = load i64, ptr %2, align 8
  %12 = load i64, ptr %10, align 8
  %13 = icmp slt i64 %11, %12
  %14 = zext i1 %13 to i8
  %15 = icmp ne i8 %14, 0
  br i1 %15, label %16, label %23

16:                                               ; preds = %9
  %17 = getelementptr %Vector, ptr %0, i32 0, i32 0
  %18 = load i64, ptr %2, align 8
  %19 = load ptr, ptr %17, align 8
  %20 = getelementptr i8, ptr %19, i64 %18
  store i8 0, ptr %20, align 1
  %21 = load i64, ptr %2, align 8
  %22 = add i64 %21, 1
  store i64 %22, ptr %2, align 8
  br label %9

23:                                               ; preds = %9
  ret void
}

define void @rl_m_init__VectorTint64_tT(ptr %0) {
  %2 = alloca i64, i64 1, align 8
  %3 = getelementptr %Vector.1, ptr %0, i32 0, i32 1
  store i64 0, ptr %3, align 8
  %4 = getelementptr %Vector.1, ptr %0, i32 0, i32 2
  store i64 4, ptr %4, align 8
  %5 = getelementptr %Vector.1, ptr %0, i32 0, i32 0
  %6 = call { i64, i1 } @llvm.umul.with.overflow.i64(i64 4, i64 8)
  %7 = extractvalue { i64, i1 } %6, 0
  %8 = call ptr @malloc(i64 %7)
  store ptr %8, ptr %5, align 8
  store i64 0, ptr %2, align 8
  br label %9

9:                                                ; preds = %16, %1
  %10 = getelementptr %Vector.1, ptr %0, i32 0, i32 2
  %11 = load i64, ptr %2, align 8
  %12 = load i64, ptr %10, align 8
  %13 = icmp slt i64 %11, %12
  %14 = zext i1 %13 to i8
  %15 = icmp ne i8 %14, 0
  br i1 %15, label %16, label %23

16:                                               ; preds = %9
  %17 = getelementptr %Vector.1, ptr %0, i32 0, i32 0
  %18 = load i64, ptr %2, align 8
  %19 = load ptr, ptr %17, align 8
  %20 = getelementptr i64, ptr %19, i64 %18
  store i64 0, ptr %20, align 8
  %21 = load i64, ptr %2, align 8
  %22 = add i64 %21, 1
  store i64 %22, ptr %2, align 8
  br label %9

23:                                               ; preds = %9
  ret void
}

define internal void @rl_m__grow__VectorTint8_tT_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca ptr, i64 1, align 8
  %5 = getelementptr %Vector, ptr %0, i32 0, i32 2
  %6 = load i64, ptr %5, align 8
  %7 = load i64, ptr %1, align 8
  %8 = icmp sgt i64 %6, %7
  %9 = zext i1 %8 to i8
  %10 = icmp ne i8 %9, 0
  br i1 %10, label %68, label %11

11:                                               ; preds = %2
  %12 = load i64, ptr %1, align 8
  %13 = mul i64 %12, 2
  %14 = call { i64, i1 } @llvm.umul.with.overflow.i64(i64 %13, i64 1)
  %15 = extractvalue { i64, i1 } %14, 0
  %16 = call ptr @malloc(i64 %15)
  store ptr %16, ptr %4, align 8
  store i64 0, ptr %3, align 8
  br label %17

17:                                               ; preds = %24, %11
  %18 = load i64, ptr %1, align 8
  %19 = mul i64 %18, 2
  %20 = load i64, ptr %3, align 8
  %21 = icmp slt i64 %20, %19
  %22 = zext i1 %21 to i8
  %23 = icmp ne i8 %22, 0
  br i1 %23, label %24, label %30

24:                                               ; preds = %17
  %25 = load i64, ptr %3, align 8
  %26 = load ptr, ptr %4, align 8
  %27 = getelementptr i8, ptr %26, i64 %25
  store i8 0, ptr %27, align 1
  %28 = load i64, ptr %3, align 8
  %29 = add i64 %28, 1
  store i64 %29, ptr %3, align 8
  br label %17

30:                                               ; preds = %17
  store i64 0, ptr %3, align 8
  br label %31

31:                                               ; preds = %38, %30
  %32 = getelementptr %Vector, ptr %0, i32 0, i32 1
  %33 = load i64, ptr %3, align 8
  %34 = load i64, ptr %32, align 8
  %35 = icmp slt i64 %33, %34
  %36 = zext i1 %35 to i8
  %37 = icmp ne i8 %36, 0
  br i1 %37, label %38, label %49

38:                                               ; preds = %31
  %39 = load i64, ptr %3, align 8
  %40 = load ptr, ptr %4, align 8
  %41 = getelementptr i8, ptr %40, i64 %39
  %42 = getelementptr %Vector, ptr %0, i32 0, i32 0
  %43 = load i64, ptr %3, align 8
  %44 = load ptr, ptr %42, align 8
  %45 = getelementptr i8, ptr %44, i64 %43
  %46 = load i8, ptr %45, align 1
  store i8 %46, ptr %41, align 1
  %47 = load i64, ptr %3, align 8
  %48 = add i64 %47, 1
  store i64 %48, ptr %3, align 8
  br label %31

49:                                               ; preds = %31
  store i64 0, ptr %3, align 8
  br label %50

50:                                               ; preds = %57, %49
  %51 = getelementptr %Vector, ptr %0, i32 0, i32 2
  %52 = load i64, ptr %3, align 8
  %53 = load i64, ptr %51, align 8
  %54 = icmp slt i64 %52, %53
  %55 = zext i1 %54 to i8
  %56 = icmp ne i8 %55, 0
  br i1 %56, label %57, label %60

57:                                               ; preds = %50
  %58 = load i64, ptr %3, align 8
  %59 = add i64 %58, 1
  store i64 %59, ptr %3, align 8
  br label %50

60:                                               ; preds = %50
  %61 = getelementptr %Vector, ptr %0, i32 0, i32 0
  %62 = load ptr, ptr %61, align 8
  call void @free(ptr %62)
  %63 = getelementptr %Vector, ptr %0, i32 0, i32 2
  %64 = load i64, ptr %1, align 8
  %65 = mul i64 %64, 2
  store i64 %65, ptr %63, align 8
  %66 = getelementptr %Vector, ptr %0, i32 0, i32 0
  %67 = load ptr, ptr %4, align 8
  store ptr %67, ptr %66, align 8
  ret void

68:                                               ; preds = %2
  ret void
}

define internal void @rl_m__grow__VectorTint64_tT_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca ptr, i64 1, align 8
  %5 = getelementptr %Vector.1, ptr %0, i32 0, i32 2
  %6 = load i64, ptr %5, align 8
  %7 = load i64, ptr %1, align 8
  %8 = icmp sgt i64 %6, %7
  %9 = zext i1 %8 to i8
  %10 = icmp ne i8 %9, 0
  br i1 %10, label %68, label %11

11:                                               ; preds = %2
  %12 = load i64, ptr %1, align 8
  %13 = mul i64 %12, 2
  %14 = call { i64, i1 } @llvm.umul.with.overflow.i64(i64 %13, i64 8)
  %15 = extractvalue { i64, i1 } %14, 0
  %16 = call ptr @malloc(i64 %15)
  store ptr %16, ptr %4, align 8
  store i64 0, ptr %3, align 8
  br label %17

17:                                               ; preds = %24, %11
  %18 = load i64, ptr %1, align 8
  %19 = mul i64 %18, 2
  %20 = load i64, ptr %3, align 8
  %21 = icmp slt i64 %20, %19
  %22 = zext i1 %21 to i8
  %23 = icmp ne i8 %22, 0
  br i1 %23, label %24, label %30

24:                                               ; preds = %17
  %25 = load i64, ptr %3, align 8
  %26 = load ptr, ptr %4, align 8
  %27 = getelementptr i64, ptr %26, i64 %25
  store i64 0, ptr %27, align 8
  %28 = load i64, ptr %3, align 8
  %29 = add i64 %28, 1
  store i64 %29, ptr %3, align 8
  br label %17

30:                                               ; preds = %17
  store i64 0, ptr %3, align 8
  br label %31

31:                                               ; preds = %38, %30
  %32 = getelementptr %Vector.1, ptr %0, i32 0, i32 1
  %33 = load i64, ptr %3, align 8
  %34 = load i64, ptr %32, align 8
  %35 = icmp slt i64 %33, %34
  %36 = zext i1 %35 to i8
  %37 = icmp ne i8 %36, 0
  br i1 %37, label %38, label %49

38:                                               ; preds = %31
  %39 = load i64, ptr %3, align 8
  %40 = load ptr, ptr %4, align 8
  %41 = getelementptr i64, ptr %40, i64 %39
  %42 = getelementptr %Vector.1, ptr %0, i32 0, i32 0
  %43 = load i64, ptr %3, align 8
  %44 = load ptr, ptr %42, align 8
  %45 = getelementptr i64, ptr %44, i64 %43
  %46 = load i64, ptr %45, align 8
  store i64 %46, ptr %41, align 8
  %47 = load i64, ptr %3, align 8
  %48 = add i64 %47, 1
  store i64 %48, ptr %3, align 8
  br label %31

49:                                               ; preds = %31
  store i64 0, ptr %3, align 8
  br label %50

50:                                               ; preds = %57, %49
  %51 = getelementptr %Vector.1, ptr %0, i32 0, i32 2
  %52 = load i64, ptr %3, align 8
  %53 = load i64, ptr %51, align 8
  %54 = icmp slt i64 %52, %53
  %55 = zext i1 %54 to i8
  %56 = icmp ne i8 %55, 0
  br i1 %56, label %57, label %60

57:                                               ; preds = %50
  %58 = load i64, ptr %3, align 8
  %59 = add i64 %58, 1
  store i64 %59, ptr %3, align 8
  br label %50

60:                                               ; preds = %50
  %61 = getelementptr %Vector.1, ptr %0, i32 0, i32 0
  %62 = load ptr, ptr %61, align 8
  call void @free(ptr %62)
  %63 = getelementptr %Vector.1, ptr %0, i32 0, i32 2
  %64 = load i64, ptr %1, align 8
  %65 = mul i64 %64, 2
  store i64 %65, ptr %63, align 8
  %66 = getelementptr %Vector.1, ptr %0, i32 0, i32 0
  %67 = load ptr, ptr %4, align 8
  store ptr %67, ptr %66, align 8
  ret void

68:                                               ; preds = %2
  ret void
}

define void @rl_m_to_indented_lines__String_r_String(ptr %0, ptr %1) {
  %3 = alloca %String, i64 1, align 8
  %4 = alloca %String, i64 1, align 8
  %5 = alloca i8, i64 1, align 1
  %6 = alloca ptr, i64 1, align 8
  %7 = alloca ptr, i64 1, align 8
  %8 = alloca i8, i64 1, align 1
  %9 = alloca ptr, i64 1, align 8
  %10 = alloca i64, i64 1, align 8
  %11 = alloca i8, i64 1, align 1
  %12 = alloca ptr, i64 1, align 8
  %13 = alloca ptr, i64 1, align 8
  %14 = alloca ptr, i64 1, align 8
  %15 = alloca i8, i64 1, align 1
  %16 = alloca ptr, i64 1, align 8
  %17 = alloca i8, i64 1, align 1
  %18 = alloca ptr, i64 1, align 8
  %19 = alloca i64, i64 1, align 8
  %20 = alloca i64, i64 1, align 8
  %21 = alloca i64, i64 1, align 8
  %22 = alloca %String, i64 1, align 8
  call void @rl_m_init__String(ptr %22)
  store i64 0, ptr %21, align 8
  store i64 0, ptr %20, align 8
  br label %23

23:                                               ; preds = %69, %2
  call void @rl_m_size__String_r_int64_t(ptr %19, ptr %1)
  %24 = load i64, ptr %21, align 8
  %25 = load i64, ptr %19, align 8
  %26 = icmp slt i64 %24, %25
  %27 = zext i1 %26 to i8
  %28 = icmp ne i8 %27, 0
  br i1 %28, label %29, label %72

29:                                               ; preds = %23
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %18, ptr %1, ptr %21)
  %30 = load ptr, ptr %18, align 8
  call void @rl_is_open_paren__int8_t_r_bool(ptr %17, ptr %30)
  %31 = load i8, ptr %17, align 1
  %32 = icmp ne i8 %31, 0
  br i1 %32, label %65, label %33

33:                                               ; preds = %29
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %16, ptr %1, ptr %21)
  %34 = load ptr, ptr %16, align 8
  call void @rl_is_close_paren__int8_t_r_bool(ptr %15, ptr %34)
  %35 = load i8, ptr %15, align 1
  %36 = icmp ne i8 %35, 0
  br i1 %36, label %60, label %37

37:                                               ; preds = %33
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %14, ptr %1, ptr %21)
  %38 = load ptr, ptr %14, align 8
  %39 = load i8, ptr %38, align 1
  %40 = icmp eq i8 %39, 44
  %41 = zext i1 %40 to i8
  %42 = icmp ne i8 %41, 0
  br i1 %42, label %45, label %43

43:                                               ; preds = %37
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %13, ptr %1, ptr %21)
  %44 = load ptr, ptr %13, align 8
  call void @rl_m_append__String_int8_t(ptr %22, ptr %44)
  br label %59

45:                                               ; preds = %37
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %12, ptr %1, ptr %21)
  %46 = load ptr, ptr %12, align 8
  call void @rl_m_append__String_int8_t(ptr %22, ptr %46)
  store i8 10, ptr %11, align 1
  call void @rl_m_append__String_int8_t(ptr %22, ptr %11)
  call void @rl__indent_string__String_int64_t(ptr %22, ptr %20)
  %47 = load i64, ptr %21, align 8
  %48 = add i64 %47, 1
  store i64 %48, ptr %10, align 8
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %9, ptr %1, ptr %10)
  %49 = load ptr, ptr %9, align 8
  %50 = load i8, ptr %49, align 1
  %51 = icmp eq i8 %50, 32
  %52 = zext i1 %51 to i8
  %53 = icmp ne i8 %52, 0
  br i1 %53, label %55, label %54

54:                                               ; preds = %45
  br label %58

55:                                               ; preds = %45
  %56 = load i64, ptr %21, align 8
  %57 = add i64 %56, 1
  store i64 %57, ptr %21, align 8
  br label %58

58:                                               ; preds = %55, %54
  br label %59

59:                                               ; preds = %58, %43
  br label %64

60:                                               ; preds = %33
  store i8 10, ptr %8, align 1
  call void @rl_m_append__String_int8_t(ptr %22, ptr %8)
  %61 = load i64, ptr %20, align 8
  %62 = sub i64 %61, 1
  store i64 %62, ptr %20, align 8
  call void @rl__indent_string__String_int64_t(ptr %22, ptr %20)
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %7, ptr %1, ptr %21)
  %63 = load ptr, ptr %7, align 8
  call void @rl_m_append__String_int8_t(ptr %22, ptr %63)
  br label %64

64:                                               ; preds = %60, %59
  br label %69

65:                                               ; preds = %29
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %6, ptr %1, ptr %21)
  %66 = load ptr, ptr %6, align 8
  call void @rl_m_append__String_int8_t(ptr %22, ptr %66)
  store i8 10, ptr %5, align 1
  call void @rl_m_append__String_int8_t(ptr %22, ptr %5)
  %67 = load i64, ptr %20, align 8
  %68 = add i64 %67, 1
  store i64 %68, ptr %20, align 8
  call void @rl__indent_string__String_int64_t(ptr %22, ptr %20)
  br label %69

69:                                               ; preds = %65, %64
  %70 = load i64, ptr %21, align 8
  %71 = add i64 %70, 1
  store i64 %71, ptr %21, align 8
  br label %23

72:                                               ; preds = %23
  call void @rl_m_init__String(ptr %4)
  call void @rl_m_assign__String_String(ptr %4, ptr %22)
  call void @llvm.memcpy.p0.p0.i64(ptr %3, ptr %4, i64 24, i1 false)
  call void @rl_m_drop__String(ptr %22)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 24, i1 false)
  ret void
}

define void @rl_m_reverse__String(ptr %0) {
  %2 = alloca ptr, i64 1, align 8
  %3 = alloca ptr, i64 1, align 8
  %4 = alloca ptr, i64 1, align 8
  %5 = alloca i8, i64 1, align 1
  %6 = alloca ptr, i64 1, align 8
  %7 = alloca i64, i64 1, align 8
  %8 = alloca i64, i64 1, align 8
  %9 = alloca i64, i64 1, align 8
  store i64 0, ptr %9, align 8
  call void @rl_m_size__String_r_int64_t(ptr %8, ptr %0)
  %10 = load i64, ptr %8, align 8
  %11 = sub i64 %10, 1
  store i64 %11, ptr %7, align 8
  br label %12

12:                                               ; preds = %18, %1
  %13 = load i64, ptr %9, align 8
  %14 = load i64, ptr %7, align 8
  %15 = icmp slt i64 %13, %14
  %16 = zext i1 %15 to i8
  %17 = icmp ne i8 %16, 0
  br i1 %17, label %18, label %34

18:                                               ; preds = %12
  %19 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef(ptr %6, ptr %19, ptr %9)
  %20 = load ptr, ptr %6, align 8
  store i8 0, ptr %5, align 1
  %21 = load i8, ptr %20, align 1
  store i8 %21, ptr %5, align 1
  %22 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef(ptr %4, ptr %22, ptr %9)
  %23 = load ptr, ptr %4, align 8
  %24 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef(ptr %3, ptr %24, ptr %7)
  %25 = load ptr, ptr %3, align 8
  %26 = load i8, ptr %25, align 1
  store i8 %26, ptr %23, align 1
  %27 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef(ptr %2, ptr %27, ptr %7)
  %28 = load ptr, ptr %2, align 8
  %29 = load i8, ptr %5, align 1
  store i8 %29, ptr %28, align 1
  %30 = load i64, ptr %9, align 8
  %31 = add i64 %30, 1
  store i64 %31, ptr %9, align 8
  %32 = load i64, ptr %7, align 8
  %33 = sub i64 %32, 1
  store i64 %33, ptr %7, align 8
  br label %12

34:                                               ; preds = %12
  ret void
}

define void @rl_m_back__String_r_int8_tRef(ptr %0, ptr %1) {
  %3 = alloca ptr, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  %5 = alloca i64, i64 1, align 8
  %6 = getelementptr %String, ptr %1, i32 0, i32 0
  %7 = getelementptr %String, ptr %1, i32 0, i32 0
  call void @rl_m_size__VectorTint8_tT_r_int64_t(ptr %5, ptr %7)
  %8 = load i64, ptr %5, align 8
  %9 = sub i64 %8, 2
  store i64 %9, ptr %4, align 8
  call void @rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef(ptr %3, ptr %6, ptr %4)
  %10 = load ptr, ptr %3, align 8
  store ptr %10, ptr %0, align 8
  ret void
}

define void @rl_m_drop_back__String_int64_t(ptr %0, ptr %1) {
  %3 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_drop_back__VectorTint8_tT_int64_t(ptr %3, ptr %1)
  ret void
}

define void @rl_m_not_equal__String_strlit_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  call void @rl_m_equal__String_strlit_r_bool(ptr %5, ptr %1, ptr %2)
  %6 = load i8, ptr %5, align 1
  %7 = icmp eq i8 %6, 0
  %8 = zext i1 %7 to i8
  store i8 %8, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define void @rl_m_not_equal__String_String_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  call void @rl_m_equal__String_String_r_bool(ptr %5, ptr %1, ptr %2)
  %6 = load i8, ptr %5, align 1
  %7 = icmp eq i8 %6, 0
  %8 = zext i1 %7 to i8
  store i8 %8, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define void @rl_m_equal__String_String_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  %6 = alloca i8, i64 1, align 1
  %7 = alloca ptr, i64 1, align 8
  %8 = alloca ptr, i64 1, align 8
  %9 = alloca i64, i64 1, align 8
  %10 = alloca i64, i64 1, align 8
  %11 = alloca i64, i64 1, align 8
  %12 = alloca i64, i64 1, align 8
  call void @rl_m_size__String_r_int64_t(ptr %12, ptr %2)
  call void @rl_m_size__String_r_int64_t(ptr %11, ptr %1)
  %13 = load i64, ptr %12, align 8
  %14 = load i64, ptr %11, align 8
  %15 = icmp ne i64 %13, %14
  %16 = zext i1 %15 to i8
  %17 = icmp ne i8 %16, 0
  br i1 %17, label %38, label %18

18:                                               ; preds = %3
  store i64 0, ptr %10, align 8
  br label %19

19:                                               ; preds = %33, %18
  call void @rl_m_size__String_r_int64_t(ptr %9, ptr %1)
  %20 = load i64, ptr %10, align 8
  %21 = load i64, ptr %9, align 8
  %22 = icmp slt i64 %20, %21
  %23 = zext i1 %22 to i8
  %24 = icmp ne i8 %23, 0
  br i1 %24, label %25, label %37

25:                                               ; preds = %19
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %8, ptr %1, ptr %10)
  %26 = load ptr, ptr %8, align 8
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %7, ptr %2, ptr %10)
  %27 = load ptr, ptr %7, align 8
  %28 = load i8, ptr %26, align 1
  %29 = load i8, ptr %27, align 1
  %30 = icmp ne i8 %28, %29
  %31 = zext i1 %30 to i8
  %32 = icmp ne i8 %31, 0
  br i1 %32, label %36, label %33

33:                                               ; preds = %25
  %34 = load i64, ptr %10, align 8
  %35 = add i64 %34, 1
  store i64 %35, ptr %10, align 8
  br label %19

36:                                               ; preds = %25
  store i8 0, ptr %6, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %6, i64 1, i1 false)
  ret void

37:                                               ; preds = %19
  store i8 1, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void

38:                                               ; preds = %3
  store i8 0, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define void @rl_m_equal__String_strlit_r_bool(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  %6 = alloca i8, i64 1, align 1
  %7 = alloca i8, i64 1, align 1
  %8 = alloca ptr, i64 1, align 8
  %9 = alloca i64, i64 1, align 8
  %10 = alloca i64, i64 1, align 8
  store i64 0, ptr %10, align 8
  br label %11

11:                                               ; preds = %35, %3
  call void @rl_m_size__String_r_int64_t(ptr %9, ptr %1)
  %12 = load i64, ptr %10, align 8
  %13 = load i64, ptr %9, align 8
  %14 = icmp slt i64 %12, %13
  %15 = zext i1 %14 to i8
  %16 = icmp ne i8 %15, 0
  br i1 %16, label %17, label %40

17:                                               ; preds = %11
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %8, ptr %1, ptr %10)
  %18 = load ptr, ptr %8, align 8
  %19 = load i64, ptr %10, align 8
  %20 = load ptr, ptr %2, align 8
  %21 = getelementptr i8, ptr %20, i64 %19
  %22 = load i8, ptr %18, align 1
  %23 = load i8, ptr %21, align 1
  %24 = icmp ne i8 %22, %23
  %25 = zext i1 %24 to i8
  %26 = icmp ne i8 %25, 0
  br i1 %26, label %39, label %27

27:                                               ; preds = %17
  %28 = load i64, ptr %10, align 8
  %29 = load ptr, ptr %2, align 8
  %30 = getelementptr i8, ptr %29, i64 %28
  %31 = load i8, ptr %30, align 1
  %32 = icmp eq i8 %31, 0
  %33 = zext i1 %32 to i8
  %34 = icmp ne i8 %33, 0
  br i1 %34, label %38, label %35

35:                                               ; preds = %27
  %36 = load i64, ptr %10, align 8
  %37 = add i64 %36, 1
  store i64 %37, ptr %10, align 8
  br label %11

38:                                               ; preds = %27
  store i8 0, ptr %7, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %7, i64 1, i1 false)
  ret void

39:                                               ; preds = %17
  store i8 0, ptr %6, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %6, i64 1, i1 false)
  ret void

40:                                               ; preds = %11
  %41 = load i64, ptr %10, align 8
  %42 = load ptr, ptr %2, align 8
  %43 = getelementptr i8, ptr %42, i64 %41
  %44 = load i8, ptr %43, align 1
  %45 = icmp ne i8 %44, 0
  %46 = zext i1 %45 to i8
  %47 = icmp ne i8 %46, 0
  br i1 %47, label %49, label %48

48:                                               ; preds = %40
  store i8 1, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void

49:                                               ; preds = %40
  store i8 0, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void
}

define void @rl_m_add__String_String_r_String(ptr %0, ptr %1, ptr %2) {
  %4 = alloca %String, i64 1, align 8
  %5 = alloca %String, i64 1, align 8
  %6 = alloca %String, i64 1, align 8
  call void @rl_m_init__String(ptr %6)
  call void @rl_m_append__String_String(ptr %6, ptr %1)
  call void @rl_m_append__String_String(ptr %6, ptr %2)
  call void @rl_m_init__String(ptr %5)
  call void @rl_m_assign__String_String(ptr %5, ptr %6)
  call void @llvm.memcpy.p0.p0.i64(ptr %4, ptr %5, i64 24, i1 false)
  call void @rl_m_drop__String(ptr %6)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 24, i1 false)
  ret void
}

define void @rl_m_append_quoted__String_String(ptr %0, ptr %1) {
  %3 = alloca i8, i64 1, align 1
  %4 = alloca i8, i64 1, align 1
  %5 = alloca ptr, i64 1, align 8
  %6 = alloca i8, i64 1, align 1
  %7 = alloca ptr, i64 1, align 8
  %8 = alloca i64, i64 1, align 8
  %9 = alloca i64, i64 1, align 8
  %10 = alloca i8, i64 1, align 1
  %11 = alloca i8, i64 1, align 1
  %12 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_pop__VectorTint8_tT_r_int8_t(ptr %11, ptr %12)
  store i8 34, ptr %10, align 1
  %13 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %13, ptr %10)
  store i64 0, ptr %9, align 8
  br label %14

14:                                               ; preds = %29, %2
  call void @rl_m_size__String_r_int64_t(ptr %8, ptr %1)
  %15 = load i64, ptr %9, align 8
  %16 = load i64, ptr %8, align 8
  %17 = icmp slt i64 %15, %16
  %18 = zext i1 %17 to i8
  %19 = icmp ne i8 %18, 0
  br i1 %19, label %20, label %34

20:                                               ; preds = %14
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %7, ptr %1, ptr %9)
  %21 = load ptr, ptr %7, align 8
  %22 = load i8, ptr %21, align 1
  %23 = icmp eq i8 %22, 34
  %24 = zext i1 %23 to i8
  %25 = icmp ne i8 %24, 0
  br i1 %25, label %27, label %26

26:                                               ; preds = %20
  br label %29

27:                                               ; preds = %20
  store i8 92, ptr %6, align 1
  %28 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %28, ptr %6)
  br label %29

29:                                               ; preds = %27, %26
  %30 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %5, ptr %1, ptr %9)
  %31 = load ptr, ptr %5, align 8
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %30, ptr %31)
  %32 = load i64, ptr %9, align 8
  %33 = add i64 %32, 1
  store i64 %33, ptr %9, align 8
  br label %14

34:                                               ; preds = %14
  store i8 34, ptr %4, align 1
  %35 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %35, ptr %4)
  store i8 0, ptr %3, align 1
  %36 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %36, ptr %3)
  ret void
}

define void @rl_m_append__String_String(ptr %0, ptr %1) {
  %3 = alloca i8, i64 1, align 1
  %4 = alloca ptr, i64 1, align 8
  %5 = alloca i64, i64 1, align 8
  %6 = alloca i64, i64 1, align 8
  %7 = alloca i8, i64 1, align 1
  %8 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_pop__VectorTint8_tT_r_int8_t(ptr %7, ptr %8)
  store i64 0, ptr %6, align 8
  br label %9

9:                                                ; preds = %15, %2
  call void @rl_m_size__String_r_int64_t(ptr %5, ptr %1)
  %10 = load i64, ptr %6, align 8
  %11 = load i64, ptr %5, align 8
  %12 = icmp slt i64 %10, %11
  %13 = zext i1 %12 to i8
  %14 = icmp ne i8 %13, 0
  br i1 %14, label %15, label %20

15:                                               ; preds = %9
  %16 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %4, ptr %1, ptr %6)
  %17 = load ptr, ptr %4, align 8
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %16, ptr %17)
  %18 = load i64, ptr %6, align 8
  %19 = add i64 %18, 1
  store i64 %19, ptr %6, align 8
  br label %9

20:                                               ; preds = %9
  store i8 0, ptr %3, align 1
  %21 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %21, ptr %3)
  ret void
}

define void @rl_m_append__String_strlit(ptr %0, ptr %1) {
  %3 = alloca i8, i64 1, align 1
  %4 = alloca i64, i64 1, align 8
  %5 = alloca i8, i64 1, align 1
  %6 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_pop__VectorTint8_tT_r_int8_t(ptr %5, ptr %6)
  store i64 0, ptr %4, align 8
  br label %7

7:                                                ; preds = %15, %2
  %8 = load i64, ptr %4, align 8
  %9 = load ptr, ptr %1, align 8
  %10 = getelementptr i8, ptr %9, i64 %8
  %11 = load i8, ptr %10, align 1
  %12 = icmp ne i8 %11, 0
  %13 = zext i1 %12 to i8
  %14 = icmp ne i8 %13, 0
  br i1 %14, label %15, label %22

15:                                               ; preds = %7
  %16 = getelementptr %String, ptr %0, i32 0, i32 0
  %17 = load i64, ptr %4, align 8
  %18 = load ptr, ptr %1, align 8
  %19 = getelementptr i8, ptr %18, i64 %17
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %16, ptr %19)
  %20 = load i64, ptr %4, align 8
  %21 = add i64 %20, 1
  store i64 %21, ptr %4, align 8
  br label %7

22:                                               ; preds = %7
  store i8 0, ptr %3, align 1
  %23 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %23, ptr %3)
  ret void
}

define void @rl_m_count__String_int8_t_r_int64_t(ptr %0, ptr %1, ptr %2) {
  %4 = alloca i64, i64 1, align 8
  %5 = alloca ptr, i64 1, align 8
  %6 = alloca i64, i64 1, align 8
  %7 = alloca i64, i64 1, align 8
  %8 = alloca i64, i64 1, align 8
  store i64 0, ptr %8, align 8
  store i64 0, ptr %7, align 8
  br label %9

9:                                                ; preds = %26, %3
  call void @rl_m_size__String_r_int64_t(ptr %6, ptr %1)
  %10 = load i64, ptr %7, align 8
  %11 = load i64, ptr %6, align 8
  %12 = icmp ne i64 %10, %11
  %13 = zext i1 %12 to i8
  %14 = icmp ne i8 %13, 0
  br i1 %14, label %15, label %29

15:                                               ; preds = %9
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %5, ptr %1, ptr %7)
  %16 = load ptr, ptr %5, align 8
  %17 = load i8, ptr %16, align 1
  %18 = load i8, ptr %2, align 1
  %19 = icmp eq i8 %17, %18
  %20 = zext i1 %19 to i8
  %21 = icmp ne i8 %20, 0
  br i1 %21, label %23, label %22

22:                                               ; preds = %15
  br label %26

23:                                               ; preds = %15
  %24 = load i64, ptr %8, align 8
  %25 = add i64 %24, 1
  store i64 %25, ptr %8, align 8
  br label %26

26:                                               ; preds = %23, %22
  %27 = load i64, ptr %7, align 8
  %28 = add i64 %27, 1
  store i64 %28, ptr %7, align 8
  br label %9

29:                                               ; preds = %9
  store i64 0, ptr %4, align 8
  %30 = load i64, ptr %8, align 8
  store i64 %30, ptr %4, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 8, i1 false)
  ret void
}

define void @rl_m_size__String_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  %5 = getelementptr %String, ptr %1, i32 0, i32 0
  call void @rl_m_size__VectorTint8_tT_r_int64_t(ptr %4, ptr %5)
  %6 = load i64, ptr %4, align 8
  %7 = sub i64 %6, 1
  store i64 %7, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define void @rl_m_substring_matches__String_strlit_int64_t_r_bool(ptr %0, ptr %1, ptr %2, ptr %3) {
  %5 = alloca i8, i64 1, align 1
  %6 = alloca i8, i64 1, align 1
  %7 = alloca i8, i64 1, align 1
  %8 = alloca ptr, i64 1, align 8
  %9 = alloca i64, i64 1, align 8
  %10 = alloca i64, i64 1, align 8
  %11 = alloca i64, i64 1, align 8
  call void @rl_m_size__String_r_int64_t(ptr %11, ptr %1)
  %12 = load i64, ptr %3, align 8
  %13 = load i64, ptr %11, align 8
  %14 = icmp sge i64 %12, %13
  %15 = zext i1 %14 to i8
  %16 = icmp ne i8 %15, 0
  br i1 %16, label %44, label %17

17:                                               ; preds = %4
  store i64 0, ptr %10, align 8
  br label %18

18:                                               ; preds = %39, %17
  %19 = load i64, ptr %10, align 8
  %20 = load ptr, ptr %2, align 8
  %21 = getelementptr i8, ptr %20, i64 %19
  %22 = load i8, ptr %21, align 1
  %23 = icmp ne i8 %22, 0
  %24 = zext i1 %23 to i8
  %25 = icmp ne i8 %24, 0
  br i1 %25, label %26, label %43

26:                                               ; preds = %18
  %27 = load i64, ptr %10, align 8
  %28 = load ptr, ptr %2, align 8
  %29 = getelementptr i8, ptr %28, i64 %27
  %30 = load i64, ptr %3, align 8
  %31 = load i64, ptr %10, align 8
  %32 = add i64 %30, %31
  store i64 %32, ptr %9, align 8
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %8, ptr %1, ptr %9)
  %33 = load ptr, ptr %8, align 8
  %34 = load i8, ptr %29, align 1
  %35 = load i8, ptr %33, align 1
  %36 = icmp ne i8 %34, %35
  %37 = zext i1 %36 to i8
  %38 = icmp ne i8 %37, 0
  br i1 %38, label %42, label %39

39:                                               ; preds = %26
  %40 = load i64, ptr %10, align 8
  %41 = add i64 %40, 1
  store i64 %41, ptr %10, align 8
  br label %18

42:                                               ; preds = %26
  store i8 0, ptr %7, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %7, i64 1, i1 false)
  ret void

43:                                               ; preds = %18
  store i8 1, ptr %6, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %6, i64 1, i1 false)
  ret void

44:                                               ; preds = %4
  store i8 0, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void
}

define void @rl_m_get__String_int64_t_r_int8_tRef(ptr %0, ptr %1, ptr %2) {
  %4 = alloca ptr, i64 1, align 8
  %5 = getelementptr %String, ptr %1, i32 0, i32 0
  call void @rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef(ptr %4, ptr %5, ptr %2)
  %6 = load ptr, ptr %4, align 8
  store ptr %6, ptr %0, align 8
  ret void
}

define void @rl_m_append__String_int8_t(ptr %0, ptr %1) {
  %3 = alloca i8, i64 1, align 1
  %4 = alloca ptr, i64 1, align 8
  %5 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_back__VectorTint8_tT_r_int8_tRef(ptr %4, ptr %5)
  %6 = load ptr, ptr %4, align 8
  %7 = load i8, ptr %1, align 1
  store i8 %7, ptr %6, align 1
  store i8 0, ptr %3, align 1
  %8 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %8, ptr %3)
  ret void
}

define void @rl_m_init__String(ptr %0) {
  %2 = alloca i8, i64 1, align 1
  %3 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_init__VectorTint8_tT(ptr %3)
  store i8 0, ptr %2, align 1
  %4 = getelementptr %String, ptr %0, i32 0, i32 0
  call void @rl_m_append__VectorTint8_tT_int8_t(ptr %4, ptr %2)
  ret void
}

define internal void @rl__indent_string__String_int64_t(ptr %0, ptr %1) {
  %3 = alloca ptr, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  store i64 0, ptr %4, align 8
  br label %5

5:                                                ; preds = %11, %2
  %6 = load i64, ptr %4, align 8
  %7 = load i64, ptr %1, align 8
  %8 = icmp ne i64 %6, %7
  %9 = zext i1 %8 to i8
  %10 = icmp ne i8 %9, 0
  br i1 %10, label %11, label %14

11:                                               ; preds = %5
  store ptr @str_13, ptr %3, align 8
  call void @rl_m_append__String_strlit(ptr %0, ptr %3)
  %12 = load i64, ptr %4, align 8
  %13 = add i64 %12, 1
  store i64 %13, ptr %4, align 8
  br label %5

14:                                               ; preds = %5
  ret void
}

define void @rl_s__strlit_r_String(ptr %0, ptr %1) {
  %3 = alloca %String, i64 1, align 8
  %4 = alloca %String, i64 1, align 8
  %5 = alloca %String, i64 1, align 8
  call void @rl_m_init__String(ptr %5)
  call void @rl_m_append__String_strlit(ptr %5, ptr %1)
  call void @rl_m_init__String(ptr %4)
  call void @rl_m_assign__String_String(ptr %4, ptr %5)
  call void @llvm.memcpy.p0.p0.i64(ptr %3, ptr %4, i64 24, i1 false)
  call void @rl_m_drop__String(ptr %5)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 24, i1 false)
  ret void
}

define void @rl_append_to_string__strlit_String(ptr %0, ptr %1) {
  call void @rl_m_append__String_strlit(ptr %1, ptr %0)
  ret void
}

declare void @rl_load_file__String_String_r_bool(ptr, ptr, ptr)

declare void @rl_append_to_string__int64_t_String(ptr, ptr)

declare void @rl_append_to_string__int8_t_String(ptr, ptr)

declare void @rl_append_to_string__double_String(ptr, ptr)

define void @rl_append_to_string__String_String(ptr %0, ptr %1) {
  call void @rl_m_append_quoted__String_String(ptr %1, ptr %0)
  ret void
}

define void @rl_append_to_string__bool_String(ptr %0, ptr %1) {
  %3 = alloca ptr, i64 1, align 8
  %4 = alloca ptr, i64 1, align 8
  %5 = load i8, ptr %0, align 1
  %6 = icmp ne i8 %5, 0
  br i1 %6, label %8, label %7

7:                                                ; preds = %2
  store ptr @str_14, ptr %4, align 8
  call void @rl_m_append__String_strlit(ptr %1, ptr %4)
  br label %9

8:                                                ; preds = %2
  store ptr @str_15, ptr %3, align 8
  call void @rl_m_append__String_strlit(ptr %1, ptr %3)
  br label %9

9:                                                ; preds = %8, %7
  ret void
}

define internal void @rl__to_string_impl__strlit_String(ptr %0, ptr %1) {
  call void @rl_append_to_string__strlit_String(ptr %0, ptr %1)
  ret void
}

define internal void @rl__to_string_impl__int64_t_String(ptr %0, ptr %1) {
  call void @rl_append_to_string__int64_t_String(ptr %0, ptr %1)
  ret void
}

define void @rl_to_string__int64_t_r_String(ptr %0, ptr %1) {
  %3 = alloca %String, i64 1, align 8
  %4 = alloca %String, i64 1, align 8
  %5 = alloca %String, i64 1, align 8
  call void @rl_m_init__String(ptr %5)
  call void @rl__to_string_impl__int64_t_String(ptr %1, ptr %5)
  call void @rl_m_init__String(ptr %4)
  call void @rl_m_assign__String_String(ptr %4, ptr %5)
  call void @llvm.memcpy.p0.p0.i64(ptr %3, ptr %4, i64 24, i1 false)
  call void @rl_m_drop__String(ptr %5)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 24, i1 false)
  ret void
}

define void @rl_is_space__int8_t_r_bool(ptr %0, ptr %1) {
  %3 = alloca i8, i64 1, align 1
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  %6 = load i8, ptr %1, align 1
  %7 = icmp eq i8 %6, 32
  %8 = zext i1 %7 to i8
  %9 = icmp ne i8 %8, 0
  br i1 %9, label %19, label %10

10:                                               ; preds = %2
  %11 = load i8, ptr %1, align 1
  %12 = icmp eq i8 %11, 10
  %13 = zext i1 %12 to i8
  %14 = icmp ne i8 %13, 0
  br i1 %14, label %20, label %15

15:                                               ; preds = %10
  %16 = load i8, ptr %1, align 1
  %17 = icmp eq i8 %16, 0
  %18 = zext i1 %17 to i8
  store i8 %18, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void

19:                                               ; preds = %2
  store i8 1, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void

20:                                               ; preds = %10
  store i8 1, ptr %3, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 1, i1 false)
  ret void
}

declare void @rl_is_alphanumeric__int8_t_r_bool(ptr, ptr)

define void @rl_is_open_paren__int8_t_r_bool(ptr %0, ptr %1) {
  %3 = alloca i8, i64 1, align 1
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  %6 = load i8, ptr %1, align 1
  %7 = icmp eq i8 %6, 40
  %8 = zext i1 %7 to i8
  %9 = icmp ne i8 %8, 0
  br i1 %9, label %19, label %10

10:                                               ; preds = %2
  %11 = load i8, ptr %1, align 1
  %12 = icmp eq i8 %11, 91
  %13 = zext i1 %12 to i8
  %14 = icmp ne i8 %13, 0
  br i1 %14, label %20, label %15

15:                                               ; preds = %10
  %16 = load i8, ptr %1, align 1
  %17 = icmp eq i8 %16, 123
  %18 = zext i1 %17 to i8
  store i8 %18, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void

19:                                               ; preds = %2
  store i8 1, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void

20:                                               ; preds = %10
  store i8 1, ptr %3, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 1, i1 false)
  ret void
}

define void @rl_is_close_paren__int8_t_r_bool(ptr %0, ptr %1) {
  %3 = alloca i8, i64 1, align 1
  %4 = alloca i8, i64 1, align 1
  %5 = alloca i8, i64 1, align 1
  %6 = load i8, ptr %1, align 1
  %7 = icmp eq i8 %6, 41
  %8 = zext i1 %7 to i8
  %9 = icmp ne i8 %8, 0
  br i1 %9, label %19, label %10

10:                                               ; preds = %2
  %11 = load i8, ptr %1, align 1
  %12 = icmp eq i8 %11, 125
  %13 = zext i1 %12 to i8
  %14 = icmp ne i8 %13, 0
  br i1 %14, label %20, label %15

15:                                               ; preds = %10
  %16 = load i8, ptr %1, align 1
  %17 = icmp eq i8 %16, 93
  %18 = zext i1 %17 to i8
  store i8 %18, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void

19:                                               ; preds = %2
  store i8 1, ptr %4, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %4, i64 1, i1 false)
  ret void

20:                                               ; preds = %10
  store i8 1, ptr %3, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 1, i1 false)
  ret void
}

declare void @rl_parse_string__int64_t_String_int64_t_r_bool(ptr, ptr, ptr, ptr)

declare void @rl_parse_string__int8_t_String_int64_t_r_bool(ptr, ptr, ptr, ptr)

declare void @rl_parse_string__double_String_int64_t_r_bool(ptr, ptr, ptr, ptr)

define internal void @rl__consume_space__String_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca i8, i64 1, align 1
  %5 = alloca ptr, i64 1, align 8
  %6 = alloca i8, i64 1, align 1
  br label %7

7:                                                ; preds = %20, %2
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %5, ptr %0, ptr %1)
  %8 = load ptr, ptr %5, align 8
  call void @rl_is_space__int8_t_r_bool(ptr %4, ptr %8)
  %9 = load i8, ptr %4, align 1
  store i8 %9, ptr %6, align 1
  %10 = load i8, ptr %6, align 1
  %11 = icmp ne i8 %10, 0
  br i1 %11, label %12, label %17

12:                                               ; preds = %7
  call void @rl_m_size__String_r_int64_t(ptr %3, ptr %0)
  %13 = load i64, ptr %1, align 8
  %14 = load i64, ptr %3, align 8
  %15 = icmp slt i64 %13, %14
  %16 = zext i1 %15 to i8
  store i8 %16, ptr %6, align 1
  br label %17

17:                                               ; preds = %12, %7
  %18 = load i8, ptr %6, align 1
  %19 = icmp ne i8 %18, 0
  br i1 %19, label %20, label %23

20:                                               ; preds = %17
  %21 = load i64, ptr %1, align 8
  %22 = add i64 %21, 1
  store i64 %22, ptr %1, align 8
  br label %7

23:                                               ; preds = %17
  ret void
}

define void @rl_length__strlit_r_int64_t(ptr %0, ptr %1) {
  %3 = alloca i64, i64 1, align 8
  %4 = alloca i64, i64 1, align 8
  store i64 0, ptr %4, align 8
  br label %5

5:                                                ; preds = %13, %2
  %6 = load i64, ptr %4, align 8
  %7 = load ptr, ptr %1, align 8
  %8 = getelementptr i8, ptr %7, i64 %6
  %9 = load i8, ptr %8, align 1
  %10 = icmp ne i8 %9, 0
  %11 = zext i1 %10 to i8
  %12 = icmp ne i8 %11, 0
  br i1 %12, label %13, label %16

13:                                               ; preds = %5
  %14 = load i64, ptr %4, align 8
  %15 = add i64 %14, 1
  store i64 %15, ptr %4, align 8
  br label %5

16:                                               ; preds = %5
  store i64 0, ptr %3, align 8
  %17 = load i64, ptr %4, align 8
  store i64 %17, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %3, i64 8, i1 false)
  ret void
}

define internal void @rl__consume_literal__String_strlit_int64_t_r_bool(ptr %0, ptr %1, ptr %2, ptr %3) {
  %5 = alloca i8, i64 1, align 1
  %6 = alloca i8, i64 1, align 1
  %7 = alloca i64, i64 1, align 8
  %8 = alloca i8, i64 1, align 1
  call void @rl__consume_space__String_int64_t(ptr %1, ptr %3)
  call void @rl_m_substring_matches__String_strlit_int64_t_r_bool(ptr %8, ptr %1, ptr %2, ptr %3)
  %9 = load i8, ptr %8, align 1
  %10 = icmp eq i8 %9, 0
  %11 = zext i1 %10 to i8
  %12 = icmp ne i8 %11, 0
  br i1 %12, label %17, label %13

13:                                               ; preds = %4
  call void @rl_length__strlit_r_int64_t(ptr %7, ptr %2)
  %14 = load i64, ptr %3, align 8
  %15 = load i64, ptr %7, align 8
  %16 = add i64 %14, %15
  store i64 %16, ptr %3, align 8
  store i8 1, ptr %6, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %6, i64 1, i1 false)
  ret void

17:                                               ; preds = %4
  store i8 0, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void
}

define internal void @rl__consume_literal_token__String_strlit_int64_t_r_bool(ptr %0, ptr %1, ptr %2, ptr %3) {
  %5 = alloca i8, i64 1, align 1
  %6 = alloca i8, i64 1, align 1
  %7 = alloca i8, i64 1, align 1
  %8 = alloca i8, i64 1, align 1
  %9 = alloca i8, i64 1, align 1
  %10 = alloca i8, i64 1, align 1
  %11 = alloca i8, i64 1, align 1
  %12 = alloca ptr, i64 1, align 8
  %13 = alloca i64, i64 1, align 8
  %14 = alloca i64, i64 1, align 8
  %15 = alloca i8, i64 1, align 1
  call void @rl__consume_space__String_int64_t(ptr %1, ptr %3)
  call void @rl_m_substring_matches__String_strlit_int64_t_r_bool(ptr %15, ptr %1, ptr %2, ptr %3)
  %16 = load i8, ptr %15, align 1
  %17 = icmp eq i8 %16, 0
  %18 = zext i1 %17 to i8
  %19 = icmp ne i8 %18, 0
  br i1 %19, label %49, label %20

20:                                               ; preds = %4
  call void @rl_length__strlit_r_int64_t(ptr %14, ptr %2)
  %21 = load i64, ptr %3, align 8
  %22 = load i64, ptr %14, align 8
  %23 = add i64 %21, %22
  store i64 %23, ptr %13, align 8
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %12, ptr %1, ptr %13)
  %24 = load ptr, ptr %12, align 8
  store i8 0, ptr %11, align 1
  %25 = load i8, ptr %24, align 1
  store i8 %25, ptr %11, align 1
  call void @rl_is_alphanumeric__int8_t_r_bool(ptr %9, ptr %11)
  %26 = load i8, ptr %9, align 1
  store i8 %26, ptr %10, align 1
  %27 = load i8, ptr %10, align 1
  %28 = icmp ne i8 %27, 0
  br i1 %28, label %41, label %29

29:                                               ; preds = %20
  %30 = load i8, ptr %11, align 1
  %31 = icmp eq i8 %30, 95
  %32 = zext i1 %31 to i8
  store i8 %32, ptr %8, align 1
  %33 = load i8, ptr %8, align 1
  %34 = icmp ne i8 %33, 0
  br i1 %34, label %39, label %35

35:                                               ; preds = %29
  %36 = load i8, ptr %11, align 1
  %37 = icmp eq i8 %36, 45
  %38 = zext i1 %37 to i8
  store i8 %38, ptr %8, align 1
  br label %39

39:                                               ; preds = %35, %29
  %40 = load i8, ptr %8, align 1
  store i8 %40, ptr %10, align 1
  br label %41

41:                                               ; preds = %39, %20
  %42 = load i8, ptr %10, align 1
  %43 = icmp ne i8 %42, 0
  br i1 %43, label %48, label %44

44:                                               ; preds = %41
  %45 = load i64, ptr %3, align 8
  %46 = load i64, ptr %14, align 8
  %47 = add i64 %45, %46
  store i64 %47, ptr %3, align 8
  store i8 1, ptr %7, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %7, i64 1, i1 false)
  ret void

48:                                               ; preds = %41
  store i8 0, ptr %6, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %6, i64 1, i1 false)
  ret void

49:                                               ; preds = %4
  store i8 0, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void
}

define void @rl_parse_string__String_String_int64_t_r_bool(ptr %0, ptr %1, ptr %2, ptr %3) {
  %5 = alloca i8, i64 1, align 1
  %6 = alloca i8, i64 1, align 1
  %7 = alloca i8, i64 1, align 1
  %8 = alloca ptr, i64 1, align 8
  %9 = alloca i8, i64 1, align 1
  %10 = alloca i8, i64 1, align 1
  %11 = alloca ptr, i64 1, align 8
  %12 = alloca i64, i64 1, align 8
  %13 = alloca ptr, i64 1, align 8
  %14 = alloca ptr, i64 1, align 8
  %15 = alloca i64, i64 1, align 8
  %16 = alloca i8, i64 1, align 1
  %17 = alloca i8, i64 1, align 1
  %18 = alloca ptr, i64 1, align 8
  %19 = alloca i64, i64 1, align 8
  %20 = alloca %String, i64 1, align 8
  %21 = alloca ptr, i64 1, align 8
  store ptr @str_16, ptr %21, align 8
  call void @rl_s__strlit_r_String(ptr %20, ptr %21)
  call void @rl_m_assign__String_String(ptr %1, ptr %20)
  call void @rl_m_drop__String(ptr %20)
  call void @rl__consume_space__String_int64_t(ptr %2, ptr %3)
  call void @rl_m_size__String_r_int64_t(ptr %19, ptr %2)
  %22 = load i64, ptr %19, align 8
  %23 = load i64, ptr %3, align 8
  %24 = icmp eq i64 %22, %23
  %25 = zext i1 %24 to i8
  %26 = icmp ne i8 %25, 0
  br i1 %26, label %80, label %27

27:                                               ; preds = %4
  store ptr @str_17, ptr %18, align 8
  call void @rl_m_substring_matches__String_strlit_int64_t_r_bool(ptr %17, ptr %2, ptr %18, ptr %3)
  %28 = load i8, ptr %17, align 1
  %29 = icmp ne i8 %28, 0
  br i1 %29, label %31, label %30

30:                                               ; preds = %27
  store i8 0, ptr %16, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %16, i64 1, i1 false)
  ret void

31:                                               ; preds = %27
  %32 = load i64, ptr %3, align 8
  %33 = add i64 %32, 1
  store i64 %33, ptr %3, align 8
  br label %34

34:                                               ; preds = %68, %72, %31
  call void @rl_m_size__String_r_int64_t(ptr %15, ptr %2)
  %35 = load i64, ptr %3, align 8
  %36 = load i64, ptr %15, align 8
  %37 = icmp ne i64 %35, %36
  %38 = zext i1 %37 to i8
  %39 = icmp ne i8 %38, 0
  br i1 %39, label %40, label %79

40:                                               ; preds = %34
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %14, ptr %2, ptr %3)
  %41 = load ptr, ptr %14, align 8
  %42 = load i8, ptr %41, align 1
  %43 = icmp eq i8 %42, 34
  %44 = zext i1 %43 to i8
  %45 = icmp ne i8 %44, 0
  br i1 %45, label %76, label %46

46:                                               ; preds = %40
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %13, ptr %2, ptr %3)
  %47 = load ptr, ptr %13, align 8
  %48 = load i8, ptr %47, align 1
  %49 = icmp eq i8 %48, 92
  %50 = zext i1 %49 to i8
  %51 = icmp ne i8 %50, 0
  br i1 %51, label %53, label %52

52:                                               ; preds = %46
  br label %72

53:                                               ; preds = %46
  %54 = load i64, ptr %3, align 8
  %55 = add i64 %54, 1
  store i64 %55, ptr %3, align 8
  call void @rl_m_size__String_r_int64_t(ptr %12, ptr %2)
  %56 = load i64, ptr %3, align 8
  %57 = load i64, ptr %12, align 8
  %58 = icmp eq i64 %56, %57
  %59 = zext i1 %58 to i8
  %60 = icmp ne i8 %59, 0
  br i1 %60, label %71, label %61

61:                                               ; preds = %53
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %11, ptr %2, ptr %3)
  %62 = load ptr, ptr %11, align 8
  %63 = load i8, ptr %62, align 1
  %64 = icmp eq i8 %63, 34
  %65 = zext i1 %64 to i8
  %66 = icmp ne i8 %65, 0
  br i1 %66, label %68, label %67

67:                                               ; preds = %61
  br label %72

68:                                               ; preds = %61
  store i8 34, ptr %10, align 1
  call void @rl_m_append__String_int8_t(ptr %1, ptr %10)
  %69 = load i64, ptr %3, align 8
  %70 = add i64 %69, 1
  store i64 %70, ptr %3, align 8
  br label %34

71:                                               ; preds = %53
  store i8 0, ptr %9, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %9, i64 1, i1 false)
  ret void

72:                                               ; preds = %67, %52
  call void @rl_m_get__String_int64_t_r_int8_tRef(ptr %8, ptr %2, ptr %3)
  %73 = load ptr, ptr %8, align 8
  call void @rl_m_append__String_int8_t(ptr %1, ptr %73)
  %74 = load i64, ptr %3, align 8
  %75 = add i64 %74, 1
  store i64 %75, ptr %3, align 8
  br label %34

76:                                               ; preds = %40
  %77 = load i64, ptr %3, align 8
  %78 = add i64 %77, 1
  store i64 %78, ptr %3, align 8
  store i8 1, ptr %7, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %7, i64 1, i1 false)
  ret void

79:                                               ; preds = %34
  store i8 0, ptr %6, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %6, i64 1, i1 false)
  ret void

80:                                               ; preds = %4
  store i8 0, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void
}

define void @rl_parse_string__bool_String_int64_t_r_bool(ptr %0, ptr %1, ptr %2, ptr %3) {
  %5 = alloca i8, i64 1, align 1
  %6 = alloca i8, i64 1, align 1
  %7 = alloca i8, i64 1, align 1
  %8 = alloca i8, i64 1, align 1
  %9 = alloca ptr, i64 1, align 8
  %10 = alloca i8, i64 1, align 1
  %11 = alloca ptr, i64 1, align 8
  %12 = alloca i64, i64 1, align 8
  call void @rl__consume_space__String_int64_t(ptr %2, ptr %3)
  call void @rl_m_size__String_r_int64_t(ptr %12, ptr %2)
  %13 = load i64, ptr %12, align 8
  %14 = load i64, ptr %3, align 8
  %15 = icmp eq i64 %13, %14
  %16 = zext i1 %15 to i8
  %17 = icmp ne i8 %16, 0
  br i1 %17, label %32, label %18

18:                                               ; preds = %4
  store ptr @str_15, ptr %11, align 8
  call void @rl_m_substring_matches__String_strlit_int64_t_r_bool(ptr %10, ptr %2, ptr %11, ptr %3)
  %19 = load i8, ptr %10, align 1
  %20 = icmp ne i8 %19, 0
  br i1 %20, label %28, label %21

21:                                               ; preds = %18
  store ptr @str_14, ptr %9, align 8
  call void @rl_m_substring_matches__String_strlit_int64_t_r_bool(ptr %8, ptr %2, ptr %9, ptr %3)
  %22 = load i8, ptr %8, align 1
  %23 = icmp ne i8 %22, 0
  br i1 %23, label %25, label %24

24:                                               ; preds = %21
  store i8 0, ptr %7, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %7, i64 1, i1 false)
  ret void

25:                                               ; preds = %21
  store i8 0, ptr %1, align 1
  %26 = load i64, ptr %3, align 8
  %27 = add i64 %26, 5
  store i64 %27, ptr %3, align 8
  br label %31

28:                                               ; preds = %18
  store i8 1, ptr %1, align 1
  %29 = load i64, ptr %3, align 8
  %30 = add i64 %29, 4
  store i64 %30, ptr %3, align 8
  br label %31

31:                                               ; preds = %28, %25
  store i8 1, ptr %6, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %6, i64 1, i1 false)
  ret void

32:                                               ; preds = %4
  store i8 0, ptr %5, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %5, i64 1, i1 false)
  ret void
}

declare void @rl_print_string__String(ptr)

declare void @rl_print_string_lit__strlit(ptr)

define void @rl_print__String(ptr %0) {
  call void @rl_print_string__String(ptr %0)
  ret void
}

define void @rl_print__strlit(ptr %0) {
  call void @rl_print_string_lit__strlit(ptr %0)
  ret void
}

define i32 @main() {
  %1 = alloca i64, i64 1, align 8
  call void @rl_main__r_int64_t(ptr %1)
  %2 = load i64, ptr %1, align 8
  %3 = trunc i64 %2 to i32
  ret i32 %3
}

define void @rl_main__r_int64_t(ptr %0) {
  %2 = alloca i64, i64 1, align 8
  %3 = alloca i64, i64 1, align 8
  %4 = alloca %String, i64 1, align 8
  %5 = alloca %String, i64 1, align 8
  %6 = alloca i64, i64 1, align 8
  %7 = alloca %String, i64 1, align 8
  %8 = alloca %String, i64 1, align 8
  %9 = alloca ptr, i64 1, align 8
  %10 = alloca %String, i64 1, align 8
  %11 = alloca %String, i64 1, align 8
  %12 = alloca %String, i64 1, align 8
  %13 = alloca ptr, i64 1, align 8
  %14 = alloca ptr, i64 1, align 8
  %15 = alloca i64, i64 1, align 8
  %16 = alloca i64, i64 1, align 8
  %17 = alloca %String, i64 1, align 8
  %18 = alloca %String, i64 1, align 8
  %19 = alloca i64, i64 1, align 8
  %20 = alloca %String, i64 1, align 8
  %21 = alloca %String, i64 1, align 8
  %22 = alloca ptr, i64 1, align 8
  %23 = alloca %String, i64 1, align 8
  %24 = alloca %String, i64 1, align 8
  %25 = alloca %String, i64 1, align 8
  %26 = alloca ptr, i64 1, align 8
  %27 = alloca ptr, i64 1, align 8
  %28 = alloca i64, i64 1, align 8
  %29 = alloca i64, i64 1, align 8
  %30 = alloca i8, i64 1, align 1
  %31 = alloca ptr, i64 1, align 8
  %32 = alloca ptr, i64 1, align 8
  %33 = alloca i64, i64 1, align 8
  %34 = alloca i64, i64 1, align 8
  %35 = alloca i64, i64 1, align 8
  %36 = alloca %Range, i64 1, align 8
  %37 = alloca i64, i64 1, align 8
  %38 = alloca %Dict, i64 1, align 8
  %39 = alloca %Vector.1, i64 1, align 8
  %40 = alloca %Vector.1, i64 1, align 8
  %41 = alloca ptr, i64 1, align 8
  %42 = alloca i8, i64 1, align 1
  %43 = alloca i8, i64 1, align 1
  %44 = alloca %String, i64 1, align 8
  %45 = alloca %String, i64 1, align 8
  %46 = alloca i64, i64 1, align 8
  %47 = alloca %String, i64 1, align 8
  %48 = alloca %String, i64 1, align 8
  %49 = alloca ptr, i64 1, align 8
  %50 = alloca %String, i64 1, align 8
  %51 = alloca %String, i64 1, align 8
  %52 = alloca %String, i64 1, align 8
  %53 = alloca ptr, i64 1, align 8
  %54 = alloca i8, i64 1, align 1
  %55 = alloca i64, i64 1, align 8
  %56 = alloca i64, i64 1, align 8
  %57 = alloca i64, i64 1, align 8
  %58 = alloca i64, i64 1, align 8
  %59 = alloca %Range, i64 1, align 8
  %60 = alloca i8, i64 1, align 1
  %61 = alloca i64, i64 1, align 8
  %62 = alloca i64, i64 1, align 8
  %63 = alloca i64, i64 1, align 8
  %64 = alloca i64, i64 1, align 8
  %65 = alloca %Range, i64 1, align 8
  %66 = alloca %Dict, i64 1, align 8
  %67 = alloca i64, i64 1, align 8
  store i64 5, ptr %67, align 8
  call void @rl_m_init__DictTint64_tTint64_tT(ptr %66)
  call void @rl_range__int64_t_r_Range(ptr %65, ptr @NUM_KEYS)
  store i64 0, ptr %64, align 8
  call void @rl_m_size__Range_r_int64_t(ptr %63, ptr %65)
  br label %68

68:                                               ; preds = %74, %1
  %69 = load i64, ptr %64, align 8
  %70 = load i64, ptr %63, align 8
  %71 = icmp ne i64 %69, %70
  %72 = zext i1 %71 to i8
  %73 = icmp ne i8 %72, 0
  br i1 %73, label %74, label %80

74:                                               ; preds = %68
  call void @rl_m_get__Range_int64_t_r_int64_t(ptr %62, ptr %65, ptr %64)
  %75 = load i64, ptr %64, align 8
  %76 = add i64 %75, 1
  store i64 %76, ptr %64, align 8
  %77 = load i64, ptr %62, align 8
  %78 = load i64, ptr %62, align 8
  %79 = mul i64 %77, %78
  store i64 %79, ptr %61, align 8
  call void @rl_m_insert__DictTint64_tTint64_tT_int64_t_int64_t_r_bool(ptr %60, ptr %66, ptr %62, ptr %61)
  br label %68

80:                                               ; preds = %68
  call void @rl_range__int64_t_r_Range(ptr %59, ptr %67)
  store i64 0, ptr %58, align 8
  call void @rl_m_size__Range_r_int64_t(ptr %57, ptr %59)
  br label %81

81:                                               ; preds = %101, %80
  %82 = load i64, ptr %58, align 8
  %83 = load i64, ptr %57, align 8
  %84 = icmp ne i64 %82, %83
  %85 = zext i1 %84 to i8
  %86 = icmp ne i8 %85, 0
  br i1 %86, label %87, label %102

87:                                               ; preds = %81
  call void @rl_m_get__Range_int64_t_r_int64_t(ptr %56, ptr %59, ptr %58)
  %88 = load i64, ptr %58, align 8
  %89 = add i64 %88, 1
  store i64 %89, ptr %58, align 8
  %90 = load i64, ptr %56, align 8
  %91 = mul i64 %90, 10
  store i64 %91, ptr %55, align 8
  call void @rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool(ptr %54, ptr %66, ptr %55)
  %92 = load i8, ptr %54, align 1
  %93 = icmp ne i8 %92, 0
  br i1 %93, label %95, label %94

94:                                               ; preds = %87
  br label %101

95:                                               ; preds = %87
  store ptr @str_18, ptr %53, align 8
  call void @rl_s__strlit_r_String(ptr %52, ptr %53)
  call void @rl_to_string__int64_t_r_String(ptr %51, ptr %55)
  call void @rl_m_add__String_String_r_String(ptr %50, ptr %52, ptr %51)
  store ptr @str_19, ptr %49, align 8
  call void @rl_s__strlit_r_String(ptr %48, ptr %49)
  call void @rl_m_add__String_String_r_String(ptr %47, ptr %50, ptr %48)
  call void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr %46, ptr %66, ptr %55)
  call void @rl_to_string__int64_t_r_String(ptr %45, ptr %46)
  call void @rl_m_add__String_String_r_String(ptr %44, ptr %47, ptr %45)
  call void @rl_print__String(ptr %44)
  call void @rl_m_drop__String(ptr %52)
  call void @rl_m_drop__String(ptr %51)
  call void @rl_m_drop__String(ptr %50)
  call void @rl_m_drop__String(ptr %48)
  call void @rl_m_drop__String(ptr %47)
  call void @rl_m_drop__String(ptr %45)
  call void @rl_m_drop__String(ptr %44)
  call void @rl_m_remove__DictTint64_tTint64_tT_int64_t_r_bool(ptr %43, ptr %66, ptr %55)
  call void @rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool(ptr %42, ptr %66, ptr %55)
  %96 = load i8, ptr %42, align 1
  %97 = icmp ne i8 %96, 0
  br i1 %97, label %99, label %98

98:                                               ; preds = %95
  br label %100

99:                                               ; preds = %95
  store ptr @str_20, ptr %41, align 8
  call void @rl_print__strlit(ptr %41)
  br label %100

100:                                              ; preds = %99, %98
  br label %101

101:                                              ; preds = %100, %94
  br label %81

102:                                              ; preds = %81
  call void @rl_m_keys__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr %40, ptr %66)
  call void @rl_m_values__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr %39, ptr %66)
  call void @rl_m_init__DictTint64_tTint64_tT(ptr %38)
  call void @rl_m_size__VectorTint64_tT_r_int64_t(ptr %37, ptr %40)
  call void @rl_range__int64_t_r_Range(ptr %36, ptr %37)
  store i64 0, ptr %35, align 8
  call void @rl_m_size__Range_r_int64_t(ptr %34, ptr %36)
  br label %103

103:                                              ; preds = %109, %102
  %104 = load i64, ptr %35, align 8
  %105 = load i64, ptr %34, align 8
  %106 = icmp ne i64 %104, %105
  %107 = zext i1 %106 to i8
  %108 = icmp ne i8 %107, 0
  br i1 %108, label %109, label %114

109:                                              ; preds = %103
  call void @rl_m_get__Range_int64_t_r_int64_t(ptr %33, ptr %36, ptr %35)
  %110 = load i64, ptr %35, align 8
  %111 = add i64 %110, 1
  store i64 %111, ptr %35, align 8
  call void @rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef(ptr %32, ptr %39, ptr %33)
  %112 = load ptr, ptr %32, align 8
  call void @rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef(ptr %31, ptr %40, ptr %33)
  %113 = load ptr, ptr %31, align 8
  call void @rl_m_insert__DictTint64_tTint64_tT_int64_t_int64_t_r_bool(ptr %30, ptr %38, ptr %112, ptr %113)
  br label %103

114:                                              ; preds = %103
  store i64 0, ptr %29, align 8
  call void @rl_m_size__VectorTint64_tT_r_int64_t(ptr %28, ptr %40)
  br label %115

115:                                              ; preds = %121, %114
  %116 = load i64, ptr %29, align 8
  %117 = load i64, ptr %28, align 8
  %118 = icmp ne i64 %116, %117
  %119 = zext i1 %118 to i8
  %120 = icmp ne i8 %119, 0
  br i1 %120, label %121, label %125

121:                                              ; preds = %115
  call void @rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef(ptr %27, ptr %40, ptr %29)
  %122 = load ptr, ptr %27, align 8
  %123 = load i64, ptr %29, align 8
  %124 = add i64 %123, 1
  store i64 %124, ptr %29, align 8
  store ptr @str_21, ptr %26, align 8
  call void @rl_s__strlit_r_String(ptr %25, ptr %26)
  call void @rl_to_string__int64_t_r_String(ptr %24, ptr %122)
  call void @rl_m_add__String_String_r_String(ptr %23, ptr %25, ptr %24)
  store ptr @str_22, ptr %22, align 8
  call void @rl_s__strlit_r_String(ptr %21, ptr %22)
  call void @rl_m_add__String_String_r_String(ptr %20, ptr %23, ptr %21)
  call void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr %19, ptr %66, ptr %122)
  call void @rl_to_string__int64_t_r_String(ptr %18, ptr %19)
  call void @rl_m_add__String_String_r_String(ptr %17, ptr %20, ptr %18)
  call void @rl_print__String(ptr %17)
  call void @rl_m_drop__String(ptr %25)
  call void @rl_m_drop__String(ptr %24)
  call void @rl_m_drop__String(ptr %23)
  call void @rl_m_drop__String(ptr %21)
  call void @rl_m_drop__String(ptr %20)
  call void @rl_m_drop__String(ptr %18)
  call void @rl_m_drop__String(ptr %17)
  br label %115

125:                                              ; preds = %115
  store i64 0, ptr %16, align 8
  call void @rl_m_size__VectorTint64_tT_r_int64_t(ptr %15, ptr %39)
  br label %126

126:                                              ; preds = %132, %125
  %127 = load i64, ptr %16, align 8
  %128 = load i64, ptr %15, align 8
  %129 = icmp ne i64 %127, %128
  %130 = zext i1 %129 to i8
  %131 = icmp ne i8 %130, 0
  br i1 %131, label %132, label %136

132:                                              ; preds = %126
  call void @rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef(ptr %14, ptr %39, ptr %16)
  %133 = load ptr, ptr %14, align 8
  %134 = load i64, ptr %16, align 8
  %135 = add i64 %134, 1
  store i64 %135, ptr %16, align 8
  store ptr @str_23, ptr %13, align 8
  call void @rl_s__strlit_r_String(ptr %12, ptr %13)
  call void @rl_to_string__int64_t_r_String(ptr %11, ptr %133)
  call void @rl_m_add__String_String_r_String(ptr %10, ptr %12, ptr %11)
  store ptr @str_22, ptr %9, align 8
  call void @rl_s__strlit_r_String(ptr %8, ptr %9)
  call void @rl_m_add__String_String_r_String(ptr %7, ptr %10, ptr %8)
  call void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr %6, ptr %38, ptr %133)
  call void @rl_to_string__int64_t_r_String(ptr %5, ptr %6)
  call void @rl_m_add__String_String_r_String(ptr %4, ptr %7, ptr %5)
  call void @rl_print__String(ptr %4)
  call void @rl_m_drop__String(ptr %12)
  call void @rl_m_drop__String(ptr %11)
  call void @rl_m_drop__String(ptr %10)
  call void @rl_m_drop__String(ptr %8)
  call void @rl_m_drop__String(ptr %7)
  call void @rl_m_drop__String(ptr %5)
  call void @rl_m_drop__String(ptr %4)
  br label %126

136:                                              ; preds = %126
  call void @rl_m_clear__DictTint64_tTint64_tT(ptr %66)
  call void @rl_m_drop__DictTint64_tTint64_tT(ptr %38)
  store i64 0, ptr %3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr %2, ptr %3, i64 8, i1 false)
  call void @rl_m_drop__DictTint64_tTint64_tT(ptr %66)
  call void @rl_m_drop__VectorTint64_tT(ptr %40)
  call void @rl_m_drop__VectorTint64_tT(ptr %39)
  call void @rl_m_drop__DictTint64_tTint64_tT(ptr %38)
  call void @llvm.memcpy.p0.p0.i64(ptr %0, ptr %2, i64 8, i1 false)
  ret void
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #0

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #1

; Function Attrs: cold noreturn nounwind memory(inaccessiblemem: write)
declare void @llvm.trap() #2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare { i64, i1 } @llvm.umul.with.overflow.i64(i64, i64) #3

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { cold noreturn nounwind memory(inaccessiblemem: write) }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
