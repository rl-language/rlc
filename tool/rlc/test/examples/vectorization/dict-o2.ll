; ModuleID = 'dict.rl'
source_filename = "dict.rl"
target datalayout = "S128-e-f80:128-i128:128-i64:64-p272:64:64:64:64-p271:32:32:32:32-f128:128-f16:16-p270:32:32:32:32-f64:64-i32:32-i16:16-i8:8-i1:8-p0:64:64:64:64"
target triple = "x86_64-unknown-linux-gnu"

%Entry = type { i8, i64, i64, i64 }
%Vector.1 = type { ptr, i64, i64 }
%String = type { %Vector }
%Vector = type { ptr, i64, i64 }
%Dict = type { ptr, i64, i64, double }

@str_21 = internal constant [19 x i8] c"dicExc: key, value\00"
@str_20 = internal constant [3 x i8] c", \00"
@str_19 = internal constant [16 x i8] c"dic: key, value\00"
@str_18 = internal constant [15 x i8] c"REMOVAL FAILED\00"
@str_17 = internal constant [7 x i8] c"value \00"
@str_16 = internal constant [22 x i8] c"Found entry with key \00"
@str_14 = internal constant [1 x i8] zeroinitializer
@str_13 = internal constant [5 x i8] c"true\00"
@str_12 = internal constant [6 x i8] c"false\00"
@str_11 = internal constant [3 x i8] c"  \00"
@str_10 = internal constant [83 x i8] c"../../../../../stdlib/collections/vector.rl:99:9 error: out of bound vector access\0A"
@str_9 = internal constant [84 x i8] c"../../../../../stdlib/collections/vector.rl:106:9 error: out of bound vector access\0A"
@str_8 = internal constant [84 x i8] c"../../../../../stdlib/collections/vector.rl:105:9 error: out of bound vector access\0A"
@str_7 = internal constant [84 x i8] c"../../../../../stdlib/collections/vector.rl:140:9 error: out of bound vector access\0A"
@str_6 = internal constant [121 x i8] c"../../../../../stdlib/collections/dictionary.rl:64:17 error: Maximum probe count exceeded - likely an implementation bug\0A"
@str_5 = internal constant [76 x i8] c"../../../../../stdlib/collections/dictionary.rl:119:17 error: key not found\0A"
@str_4 = internal constant [76 x i8] c"../../../../../stdlib/collections/dictionary.rl:125:21 error: key not found\0A"
@str_3 = internal constant [127 x i8] c"../../../../../stdlib/collections/dictionary.rl:113:17 error: GET: Maximum probe count exceeded - likely an implementation bug\0A"
@str_2 = internal constant [96 x i8] c"../../../../../stdlib/collections/dictionary.rl:103:13 error: key not found in empty dictionary\0A"
@str_1 = internal constant [132 x i8] c"../../../../../stdlib/collections/dictionary.rl:145:17 error: CONTAINS: Maximum probe count exceeded - likely an implementation bug\0A"
@str_0 = internal constant [130 x i8] c"../../../../../stdlib/collections/dictionary.rl:172:17 error: REMOVE: Maximum probe count exceeded - likely an implementation bug\0A"

; Function Attrs: mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite)
declare noalias noundef ptr @malloc(i64 noundef) local_unnamed_addr #0

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #1

; Function Attrs: mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite)
declare void @free(ptr allocptr nocapture noundef) local_unnamed_addr #2

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write)
define void @rl_m_init__EntryTint64_tTint64_tT(ptr nocapture writeonly %0) local_unnamed_addr #3 {
  store i8 0, ptr %0, align 1
  %2 = getelementptr i8, ptr %0, i64 8
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %2, i8 0, i64 24, i1 false)
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_assign__DictTint64_tTint64_tT_DictTint64_tTint64_tT(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load ptr, ptr %1, align 8
  store ptr %3, ptr %0, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = getelementptr i8, ptr %1, i64 8
  %6 = load i64, ptr %5, align 8
  store i64 %6, ptr %4, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = getelementptr i8, ptr %1, i64 16
  %9 = load i64, ptr %8, align 8
  store i64 %9, ptr %7, align 8
  %10 = getelementptr i8, ptr %0, i64 24
  %11 = getelementptr i8, ptr %1, i64 24
  %12 = load double, ptr %11, align 8
  store double %12, ptr %10, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_assign__EntryTint64_tTint64_tT_EntryTint64_tTint64_tT(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i8, ptr %1, align 1
  store i8 %3, ptr %0, align 1
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = getelementptr i8, ptr %1, i64 8
  %6 = load i64, ptr %5, align 8
  store i64 %6, ptr %4, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = getelementptr i8, ptr %1, i64 16
  %9 = load i64, ptr %8, align 8
  store i64 %9, ptr %7, align 8
  %10 = getelementptr i8, ptr %0, i64 24
  %11 = getelementptr i8, ptr %1, i64 24
  %12 = load i64, ptr %11, align 8
  store i64 %12, ptr %10, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write)
define void @rl_m_init__strlit(ptr nocapture writeonly %0) local_unnamed_addr #3 {
  store i64 0, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write)
define void @rl_m_init__Range(ptr nocapture writeonly %0) local_unnamed_addr #3 {
  store i64 0, ptr %0, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write)
define void @rl_m_init__Nothing(ptr nocapture writeonly %0) local_unnamed_addr #3 {
  store i8 0, ptr %0, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_assign__String_String(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  tail call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr %0, ptr %1)
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_assign__Range_Range(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i64, ptr %1, align 8
  store i64 %3, ptr %0, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_assign__Nothing_Nothing(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i8, ptr %1, align 1
  store i8 %3, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nounwind willreturn
define void @rl_m_drop__String(ptr nocapture %0) local_unnamed_addr #6 {
  %2 = getelementptr i8, ptr %0, i64 16
  %3 = load i64, ptr %2, align 8
  %.not2.i = icmp eq i64 %3, 0
  br i1 %.not2.i, label %rl_m_drop__VectorTint8_tT.exit, label %4

4:                                                ; preds = %1
  %5 = load ptr, ptr %0, align 8
  tail call void @free(ptr %5)
  br label %rl_m_drop__VectorTint8_tT.exit

rl_m_drop__VectorTint8_tT.exit:                   ; preds = %1, %4
  %6 = getelementptr i8, ptr %0, i64 8
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %6, i8 0, i64 16, i1 false)
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_hash__int64_t_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i64, ptr %1, align 8
  %4 = lshr i64 %3, 33
  %5 = xor i64 %4, %3
  %6 = mul i64 %5, 1099511628211
  %7 = lshr i64 %6, 33
  %8 = xor i64 %7, %6
  %9 = mul i64 %8, 16777619
  %10 = lshr i64 %9, 33
  %.masked = and i64 %9, 9223372036854775807
  %11 = xor i64 %.masked, %10
  store i64 %11, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_hash__double_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load double, ptr %1, align 8
  %4 = fmul double %3, 1.000000e+06
  %5 = fptosi double %4 to i64
  %6 = lshr i64 %5, 33
  %7 = xor i64 %6, %5
  %8 = mul i64 %7, 1099511628211
  %9 = lshr i64 %8, 33
  %10 = xor i64 %9, %8
  %11 = mul i64 %10, 16777619
  %12 = lshr i64 %11, 33
  %.masked = and i64 %11, 9223372036854775807
  %13 = xor i64 %.masked, %12
  store i64 %13, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_hash__bool_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
common.ret:
  %2 = load i8, ptr %1, align 1
  %.not = icmp eq i8 %2, 0
  %. = select i1 %.not, i64 2023011127830240574, i64 1321005721090711325
  store i64 %., ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_hash__int8_t_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i8, ptr %1, align 1
  %4 = zext i8 %3 to i64
  %5 = shl nuw nsw i64 %4, 16
  %6 = or disjoint i64 %5, %4
  %7 = mul nuw nsw i64 %6, 72955717
  %8 = lshr i64 %7, 16
  %9 = xor i64 %8, %7
  %10 = mul i64 %9, 72955717
  %11 = mul i64 %9, 4781225869312
  %12 = xor i64 %10, %11
  %13 = mul i64 %12, 72955717
  %14 = and i64 %13, 9223372036854775807
  store i64 %14, ptr %0, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_compute_hash__String_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %1, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = add i64 %4, -1
  %6 = icmp sgt i64 %5, 0
  br i1 %6, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %2
  %smax = tail call i64 @llvm.smax.i64(i64 %4, i64 0)
  %7 = add i64 %4, -2
  %.not.not = icmp ugt i64 %smax, %7
  br i1 %.not.not, label %.lr.ph.preheader.split, label %16

.lr.ph.preheader.split:                           ; preds = %.lr.ph.preheader
  %.pre = load ptr, ptr %1, align 8
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader.split, %.lr.ph
  %.06 = phi i64 [ %13, %.lr.ph ], [ 2166136261, %.lr.ph.preheader.split ]
  %storemerge5 = phi i64 [ %14, %.lr.ph ], [ 0, %.lr.ph.preheader.split ]
  %8 = getelementptr i8, ptr %.pre, i64 %storemerge5
  %9 = load i8, ptr %8, align 1
  %10 = sext i8 %9 to i64
  %11 = xor i64 %.06, %10
  %12 = mul i64 %11, 16777619
  %13 = and i64 %12, 9223372036854775807
  %14 = add nuw nsw i64 %storemerge5, 1
  %15 = icmp slt i64 %14, %5
  br i1 %15, label %.lr.ph, label %._crit_edge

16:                                               ; preds = %.lr.ph.preheader
  %17 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

._crit_edge:                                      ; preds = %.lr.ph, %2
  %.0.lcssa = phi i64 [ 2166136261, %2 ], [ %13, %.lr.ph ]
  store i64 %.0.lcssa, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_hash_of__int64_t_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i64, ptr %1, align 8
  %4 = lshr i64 %3, 33
  %5 = xor i64 %4, %3
  %6 = mul i64 %5, 1099511628211
  %7 = lshr i64 %6, 33
  %8 = xor i64 %7, %6
  %9 = mul i64 %8, 16777619
  %10 = lshr i64 %9, 33
  %.masked.i.i = and i64 %9, 9223372036854775807
  %11 = xor i64 %.masked.i.i, %10
  store i64 %11, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_equal__int64_t_int64_t_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #4 {
  %4 = load i64, ptr %1, align 8
  %5 = load i64, ptr %2, align 8
  %6 = icmp eq i64 %4, %5
  %7 = zext i1 %6 to i8
  store i8 %7, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_equal__double_double_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #4 {
  %4 = load double, ptr %1, align 8
  %5 = load double, ptr %2, align 8
  %6 = fcmp oeq double %4, %5
  %7 = zext i1 %6 to i8
  store i8 %7, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_equal__bool_bool_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #4 {
  %4 = load i8, ptr %1, align 1
  %5 = load i8, ptr %2, align 1
  %6 = icmp eq i8 %4, %5
  %7 = zext i1 %6 to i8
  store i8 %7, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_equal__int8_t_int8_t_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #4 {
  %4 = load i8, ptr %1, align 1
  %5 = load i8, ptr %2, align 1
  %6 = icmp eq i8 %4, %5
  %7 = zext i1 %6 to i8
  store i8 %7, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_equal_of__int64_t_int64_t_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #4 {
  %4 = load i64, ptr %1, align 8
  %5 = load i64, ptr %2, align 8
  %6 = icmp eq i64 %4, %5
  %7 = zext i1 %6 to i8
  store i8 %7, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nounwind willreturn
define void @rl_m_drop__DictTint64_tTint64_tT(ptr nocapture %0) local_unnamed_addr #6 {
  %2 = getelementptr i8, ptr %0, i64 16
  %3 = load i64, ptr %2, align 8
  %.not = icmp eq i64 %3, 0
  br i1 %.not, label %6, label %4

4:                                                ; preds = %1
  %5 = load ptr, ptr %0, align 8
  tail call void @free(ptr %5)
  br label %6

6:                                                ; preds = %1, %4
  %7 = getelementptr i8, ptr %0, i64 8
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %7, i8 0, i64 16, i1 false)
  ret void
}

; Function Attrs: nounwind
define void @rl_m_clear__DictTint64_tTint64_tT(ptr nocapture %0) local_unnamed_addr #5 {
.lr.ph.preheader:
  %1 = getelementptr i8, ptr %0, i64 16
  %2 = load ptr, ptr %0, align 8
  tail call void @free(ptr %2)
  store i64 4, ptr %1, align 8
  %3 = getelementptr i8, ptr %0, i64 8
  store i64 0, ptr %3, align 8
  %4 = tail call dereferenceable_or_null(128) ptr @malloc(i64 128)
  store ptr %4, ptr %0, align 8
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %.02 = phi i64 [ %7, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %5 = load ptr, ptr %0, align 8
  %6 = getelementptr %Entry, ptr %5, i64 %.02
  store i8 0, ptr %6, align 1
  %7 = add nuw nsw i64 %.02, 1
  %8 = load i64, ptr %1, align 8
  %9 = icmp slt i64 %7, %8
  br i1 %9, label %.lr.ph, label %._crit_edge

._crit_edge:                                      ; preds = %.lr.ph
  ret void
}

; Function Attrs: nounwind
define void @rl_m_values__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
rl_m_init__VectorTint64_tT.exit.preheader:
  %2 = alloca %Vector.1, align 8
  %3 = alloca %Vector.1, align 8
  %4 = getelementptr inbounds i8, ptr %3, i64 8
  store i64 0, ptr %4, align 8
  %5 = getelementptr inbounds i8, ptr %3, i64 16
  store i64 4, ptr %5, align 8
  %calloc18 = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  store ptr %calloc18, ptr %3, align 8
  %6 = getelementptr i8, ptr %1, i64 8
  %7 = load i64, ptr %6, align 8
  %8 = icmp sgt i64 %7, 0
  br i1 %8, label %.lr.ph, label %rl_m_init__VectorTint64_tT.exit._crit_edge

.lr.ph:                                           ; preds = %rl_m_init__VectorTint64_tT.exit.preheader, %rl_m_init__VectorTint64_tT.exit
  %9 = phi i64 [ %35, %rl_m_init__VectorTint64_tT.exit ], [ %7, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.013 = phi i64 [ %38, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.0112 = phi i64 [ %.1, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %10 = phi i64 [ %37, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %11 = phi i64 [ %36, %rl_m_init__VectorTint64_tT.exit ], [ 4, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.pre2.i1011 = phi ptr [ %.pre2.i9, %rl_m_init__VectorTint64_tT.exit ], [ %calloc18, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %12 = load ptr, ptr %1, align 8
  %13 = getelementptr %Entry, ptr %12, i64 %.013
  %14 = load i8, ptr %13, align 1
  %.not = icmp eq i8 %14, 0
  br i1 %.not, label %rl_m_init__VectorTint64_tT.exit, label %15

15:                                               ; preds = %.lr.ph
  %16 = getelementptr i8, ptr %13, i64 24
  %17 = add i64 %10, 1
  %18 = icmp sgt i64 %11, %17
  br i1 %18, label %rl_m_append__VectorTint64_tT_int64_t.exit, label %19

19:                                               ; preds = %15
  %20 = shl i64 %17, 4
  %21 = tail call ptr @malloc(i64 %20)
  %22 = trunc i64 %17 to i63
  %23 = icmp sgt i63 %22, 0
  br i1 %23, label %.lr.ph.preheader.i.i, label %.preheader2.i.i

.lr.ph.preheader.i.i:                             ; preds = %19
  tail call void @llvm.memset.p0.i64(ptr align 8 %21, i8 0, i64 %20, i1 false)
  br label %.preheader2.i.i

.preheader2.i.i:                                  ; preds = %.lr.ph.preheader.i.i, %19
  %24 = icmp sgt i64 %10, 0
  br i1 %24, label %.lr.ph5.i.i, label %.preheader.i.i

.preheader.i.i:                                   ; preds = %.lr.ph5.i.i, %.preheader2.i.i
  tail call void @free(ptr %.pre2.i1011)
  %25 = shl i64 %17, 1
  br label %rl_m_append__VectorTint64_tT_int64_t.exit

.lr.ph5.i.i:                                      ; preds = %.preheader2.i.i, %.lr.ph5.i.i
  %.14.i.i = phi i64 [ %29, %.lr.ph5.i.i ], [ 0, %.preheader2.i.i ]
  %26 = getelementptr i64, ptr %21, i64 %.14.i.i
  %27 = getelementptr i64, ptr %.pre2.i1011, i64 %.14.i.i
  %28 = load i64, ptr %27, align 8
  store i64 %28, ptr %26, align 8
  %29 = add nuw nsw i64 %.14.i.i, 1
  %30 = icmp slt i64 %29, %10
  br i1 %30, label %.lr.ph5.i.i, label %.preheader.i.i

rl_m_append__VectorTint64_tT_int64_t.exit:        ; preds = %15, %.preheader.i.i
  %.pre2.i8 = phi ptr [ %21, %.preheader.i.i ], [ %.pre2.i1011, %15 ]
  %31 = phi i64 [ %25, %.preheader.i.i ], [ %11, %15 ]
  %32 = getelementptr i64, ptr %.pre2.i8, i64 %10
  %33 = load i64, ptr %16, align 8
  store i64 %33, ptr %32, align 8
  %34 = add i64 %.0112, 1
  %.pre = load i64, ptr %6, align 8
  br label %rl_m_init__VectorTint64_tT.exit

rl_m_init__VectorTint64_tT.exit:                  ; preds = %.lr.ph, %rl_m_append__VectorTint64_tT_int64_t.exit
  %35 = phi i64 [ %.pre, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %9, %.lr.ph ]
  %.pre2.i9 = phi ptr [ %.pre2.i8, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %.pre2.i1011, %.lr.ph ]
  %36 = phi i64 [ %31, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %11, %.lr.ph ]
  %37 = phi i64 [ %17, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %10, %.lr.ph ]
  %.1 = phi i64 [ %34, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %.0112, %.lr.ph ]
  %38 = add i64 %.013, 1
  %39 = icmp slt i64 %.1, %35
  br i1 %39, label %.lr.ph, label %rl_m_init__VectorTint64_tT.exit._crit_edge

rl_m_init__VectorTint64_tT.exit._crit_edge:       ; preds = %rl_m_init__VectorTint64_tT.exit, %rl_m_init__VectorTint64_tT.exit.preheader
  %40 = phi ptr [ %calloc18, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %.pre2.i9, %rl_m_init__VectorTint64_tT.exit ]
  %.lcssa6 = phi i64 [ 4, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %36, %rl_m_init__VectorTint64_tT.exit ]
  %.lcssa = phi i64 [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %37, %rl_m_init__VectorTint64_tT.exit ]
  store i64 %.lcssa, ptr %4, align 8
  store i64 %.lcssa6, ptr %5, align 8
  store ptr %40, ptr %3, align 8
  %41 = getelementptr inbounds i8, ptr %2, i64 8
  store i64 0, ptr %41, align 8
  %42 = getelementptr inbounds i8, ptr %2, i64 16
  store i64 4, ptr %42, align 8
  %calloc = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  store ptr %calloc, ptr %2, align 8
  call void @rl_m_assign__VectorTint64_tT_VectorTint64_tT(ptr nonnull %2, ptr nonnull %3)
  %.not2.i = icmp eq i64 %.lcssa6, 0
  br i1 %.not2.i, label %rl_m_drop__VectorTint64_tT.exit, label %43

43:                                               ; preds = %rl_m_init__VectorTint64_tT.exit._crit_edge
  tail call void @free(ptr %40)
  br label %rl_m_drop__VectorTint64_tT.exit

rl_m_drop__VectorTint64_tT.exit:                  ; preds = %rl_m_init__VectorTint64_tT.exit._crit_edge, %43
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 1 dereferenceable(24) %0, ptr noundef nonnull align 8 dereferenceable(24) %2, i64 24, i1 false)
  ret void
}

; Function Attrs: nounwind
define void @rl_m_keys__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
rl_m_init__VectorTint64_tT.exit.preheader:
  %2 = alloca %Vector.1, align 8
  %3 = alloca %Vector.1, align 8
  %4 = getelementptr inbounds i8, ptr %3, i64 8
  store i64 0, ptr %4, align 8
  %5 = getelementptr inbounds i8, ptr %3, i64 16
  store i64 4, ptr %5, align 8
  %calloc18 = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  store ptr %calloc18, ptr %3, align 8
  %6 = getelementptr i8, ptr %1, i64 8
  %7 = load i64, ptr %6, align 8
  %8 = icmp sgt i64 %7, 0
  br i1 %8, label %.lr.ph, label %rl_m_init__VectorTint64_tT.exit._crit_edge

.lr.ph:                                           ; preds = %rl_m_init__VectorTint64_tT.exit.preheader, %rl_m_init__VectorTint64_tT.exit
  %9 = phi i64 [ %35, %rl_m_init__VectorTint64_tT.exit ], [ %7, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.013 = phi i64 [ %38, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.0112 = phi i64 [ %.1, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %10 = phi i64 [ %37, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %11 = phi i64 [ %36, %rl_m_init__VectorTint64_tT.exit ], [ 4, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.pre2.i1011 = phi ptr [ %.pre2.i9, %rl_m_init__VectorTint64_tT.exit ], [ %calloc18, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %12 = load ptr, ptr %1, align 8
  %13 = getelementptr %Entry, ptr %12, i64 %.013
  %14 = load i8, ptr %13, align 1
  %.not = icmp eq i8 %14, 0
  br i1 %.not, label %rl_m_init__VectorTint64_tT.exit, label %15

15:                                               ; preds = %.lr.ph
  %16 = getelementptr i8, ptr %13, i64 16
  %17 = add i64 %10, 1
  %18 = icmp sgt i64 %11, %17
  br i1 %18, label %rl_m_append__VectorTint64_tT_int64_t.exit, label %19

19:                                               ; preds = %15
  %20 = shl i64 %17, 4
  %21 = tail call ptr @malloc(i64 %20)
  %22 = trunc i64 %17 to i63
  %23 = icmp sgt i63 %22, 0
  br i1 %23, label %.lr.ph.preheader.i.i, label %.preheader2.i.i

.lr.ph.preheader.i.i:                             ; preds = %19
  tail call void @llvm.memset.p0.i64(ptr align 8 %21, i8 0, i64 %20, i1 false)
  br label %.preheader2.i.i

.preheader2.i.i:                                  ; preds = %.lr.ph.preheader.i.i, %19
  %24 = icmp sgt i64 %10, 0
  br i1 %24, label %.lr.ph5.i.i, label %.preheader.i.i

.preheader.i.i:                                   ; preds = %.lr.ph5.i.i, %.preheader2.i.i
  tail call void @free(ptr %.pre2.i1011)
  %25 = shl i64 %17, 1
  br label %rl_m_append__VectorTint64_tT_int64_t.exit

.lr.ph5.i.i:                                      ; preds = %.preheader2.i.i, %.lr.ph5.i.i
  %.14.i.i = phi i64 [ %29, %.lr.ph5.i.i ], [ 0, %.preheader2.i.i ]
  %26 = getelementptr i64, ptr %21, i64 %.14.i.i
  %27 = getelementptr i64, ptr %.pre2.i1011, i64 %.14.i.i
  %28 = load i64, ptr %27, align 8
  store i64 %28, ptr %26, align 8
  %29 = add nuw nsw i64 %.14.i.i, 1
  %30 = icmp slt i64 %29, %10
  br i1 %30, label %.lr.ph5.i.i, label %.preheader.i.i

rl_m_append__VectorTint64_tT_int64_t.exit:        ; preds = %15, %.preheader.i.i
  %.pre2.i8 = phi ptr [ %21, %.preheader.i.i ], [ %.pre2.i1011, %15 ]
  %31 = phi i64 [ %25, %.preheader.i.i ], [ %11, %15 ]
  %32 = getelementptr i64, ptr %.pre2.i8, i64 %10
  %33 = load i64, ptr %16, align 8
  store i64 %33, ptr %32, align 8
  %34 = add i64 %.0112, 1
  %.pre = load i64, ptr %6, align 8
  br label %rl_m_init__VectorTint64_tT.exit

rl_m_init__VectorTint64_tT.exit:                  ; preds = %.lr.ph, %rl_m_append__VectorTint64_tT_int64_t.exit
  %35 = phi i64 [ %.pre, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %9, %.lr.ph ]
  %.pre2.i9 = phi ptr [ %.pre2.i8, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %.pre2.i1011, %.lr.ph ]
  %36 = phi i64 [ %31, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %11, %.lr.ph ]
  %37 = phi i64 [ %17, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %10, %.lr.ph ]
  %.1 = phi i64 [ %34, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %.0112, %.lr.ph ]
  %38 = add i64 %.013, 1
  %39 = icmp slt i64 %.1, %35
  br i1 %39, label %.lr.ph, label %rl_m_init__VectorTint64_tT.exit._crit_edge

rl_m_init__VectorTint64_tT.exit._crit_edge:       ; preds = %rl_m_init__VectorTint64_tT.exit, %rl_m_init__VectorTint64_tT.exit.preheader
  %40 = phi ptr [ %calloc18, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %.pre2.i9, %rl_m_init__VectorTint64_tT.exit ]
  %.lcssa6 = phi i64 [ 4, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %36, %rl_m_init__VectorTint64_tT.exit ]
  %.lcssa = phi i64 [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %37, %rl_m_init__VectorTint64_tT.exit ]
  store i64 %.lcssa, ptr %4, align 8
  store i64 %.lcssa6, ptr %5, align 8
  store ptr %40, ptr %3, align 8
  %41 = getelementptr inbounds i8, ptr %2, i64 8
  store i64 0, ptr %41, align 8
  %42 = getelementptr inbounds i8, ptr %2, i64 16
  store i64 4, ptr %42, align 8
  %calloc = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  store ptr %calloc, ptr %2, align 8
  call void @rl_m_assign__VectorTint64_tT_VectorTint64_tT(ptr nonnull %2, ptr nonnull %3)
  %.not2.i = icmp eq i64 %.lcssa6, 0
  br i1 %.not2.i, label %rl_m_drop__VectorTint64_tT.exit, label %43

43:                                               ; preds = %rl_m_init__VectorTint64_tT.exit._crit_edge
  tail call void @free(ptr %40)
  br label %rl_m_drop__VectorTint64_tT.exit

rl_m_drop__VectorTint64_tT.exit:                  ; preds = %rl_m_init__VectorTint64_tT.exit._crit_edge, %43
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 1 dereferenceable(24) %0, ptr noundef nonnull align 8 dereferenceable(24) %2, i64 24, i1 false)
  ret void
}

; Function Attrs: nounwind
define void @rl_m_remove__DictTint64_tTint64_tT_int64_t_r_bool(ptr nocapture writeonly %0, ptr nocapture %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = load i64, ptr %2, align 8
  %5 = lshr i64 %4, 33
  %6 = xor i64 %5, %4
  %7 = mul i64 %6, 1099511628211
  %8 = lshr i64 %7, 33
  %9 = xor i64 %8, %7
  %10 = mul i64 %9, 16777619
  %11 = lshr i64 %10, 33
  %.masked.i.i.i = and i64 %10, 9223372036854775807
  %12 = xor i64 %.masked.i.i.i, %11
  %13 = getelementptr i8, ptr %1, i64 16
  %14 = load i64, ptr %13, align 8
  %.not22 = icmp sgt i64 %14, 0
  br i1 %.not22, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %3
  %15 = load ptr, ptr %1, align 8
  br label %16

16:                                               ; preds = %.lr.ph, %33
  %.pn = phi i64 [ %12, %.lr.ph ], [ %34, %33 ]
  %.0324 = phi i64 [ 0, %.lr.ph ], [ %17, %33 ]
  %.0525 = srem i64 %.pn, %14
  %17 = add nuw nsw i64 %.0324, 1
  %18 = getelementptr %Entry, ptr %15, i64 %.0525
  %19 = load i8, ptr %18, align 1
  %20 = icmp eq i8 %19, 0
  br i1 %20, label %common.ret, label %21

21:                                               ; preds = %16
  %22 = getelementptr i8, ptr %18, i64 8
  %23 = load i64, ptr %22, align 8
  %24 = icmp eq i64 %23, %12
  br i1 %24, label %25, label %.thread

25:                                               ; preds = %21
  %26 = getelementptr i8, ptr %18, i64 16
  %27 = load i64, ptr %26, align 8
  %.not15 = icmp eq i64 %27, %4
  br i1 %.not15, label %35, label %.thread

.thread:                                          ; preds = %21, %25
  %28 = add i64 %.0525, %14
  %29 = srem i64 %23, %14
  %30 = sub i64 %28, %29
  %31 = srem i64 %30, %14
  %32 = icmp slt i64 %31, %.0324
  br i1 %32, label %common.ret, label %33

33:                                               ; preds = %.thread
  %34 = add i64 %.0525, 1
  %.not = icmp slt i64 %17, %14
  br i1 %.not, label %16, label %._crit_edge

35:                                               ; preds = %25
  %36 = getelementptr i8, ptr %1, i64 8
  %37 = load i64, ptr %36, align 8
  %38 = add i64 %37, -1
  store i64 %38, ptr %36, align 8
  %39 = add i64 %.0525, 1
  %40 = srem i64 %39, %14
  %41 = getelementptr %Entry, ptr %15, i64 %40
  %42 = load i8, ptr %41, align 1
  %43 = icmp eq i8 %42, 0
  br i1 %43, label %._crit_edge30, label %.lr.ph29

.lr.ph29:                                         ; preds = %35, %53
  %44 = phi i64 [ %61, %53 ], [ %14, %35 ]
  %.pn32 = phi ptr [ %64, %53 ], [ %41, %35 ]
  %45 = phi i8 [ %65, %53 ], [ %42, %35 ]
  %46 = phi ptr [ %63, %53 ], [ %15, %35 ]
  %.027 = phi i64 [ %.0126, %53 ], [ %.0525, %35 ]
  %.0126 = phi i64 [ %62, %53 ], [ %40, %35 ]
  %.in35 = getelementptr i8, ptr %.pn32, i64 8
  %47 = load i64, ptr %.in35, align 8
  %48 = add i64 %44, %.0126
  %49 = srem i64 %47, %44
  %50 = sub i64 %48, %49
  %51 = srem i64 %50, %44
  %52 = icmp eq i64 %51, 0
  br i1 %52, label %67, label %53

53:                                               ; preds = %.lr.ph29
  %.in33 = getelementptr i8, ptr %.pn32, i64 16
  %54 = load i64, ptr %.in33, align 8
  %.in = getelementptr i8, ptr %.pn32, i64 24
  %55 = load i64, ptr %.in, align 8
  %56 = getelementptr %Entry, ptr %46, i64 %.027
  store i8 %45, ptr %56, align 1
  %57 = getelementptr i8, ptr %56, i64 8
  store i64 %47, ptr %57, align 8
  %58 = getelementptr i8, ptr %56, i64 16
  store i64 %54, ptr %58, align 8
  %59 = getelementptr i8, ptr %56, i64 24
  store i64 %55, ptr %59, align 8
  %60 = add i64 %.0126, 1
  %61 = load i64, ptr %13, align 8
  %62 = srem i64 %60, %61
  %63 = load ptr, ptr %1, align 8
  %64 = getelementptr %Entry, ptr %63, i64 %62
  %65 = load i8, ptr %64, align 1
  %66 = icmp eq i8 %65, 0
  br i1 %66, label %._crit_edge30, label %.lr.ph29

67:                                               ; preds = %.lr.ph29
  %68 = getelementptr %Entry, ptr %46, i64 %.027
  br label %common.ret.sink.split

._crit_edge30:                                    ; preds = %53, %35
  %.0.lcssa = phi i64 [ %.0525, %35 ], [ %.0126, %53 ]
  %.lcssa = phi ptr [ %15, %35 ], [ %63, %53 ]
  %69 = getelementptr %Entry, ptr %.lcssa, i64 %.0.lcssa
  br label %common.ret.sink.split

common.ret.sink.split:                            ; preds = %._crit_edge30, %67
  %.sink = phi ptr [ %68, %67 ], [ %69, %._crit_edge30 ]
  store i8 0, ptr %.sink, align 1
  br label %common.ret

common.ret:                                       ; preds = %.thread, %16, %common.ret.sink.split
  %storemerge = phi i8 [ 1, %common.ret.sink.split ], [ 0, %16 ], [ 0, %.thread ]
  store i8 %storemerge, ptr %0, align 1
  ret void

._crit_edge:                                      ; preds = %33, %3
  %70 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_0)
  tail call void @llvm.trap()
  unreachable
}

; Function Attrs: nounwind
define void @rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = getelementptr i8, ptr %1, i64 8
  %5 = load i64, ptr %4, align 8
  %6 = icmp eq i64 %5, 0
  br i1 %6, label %common.ret, label %7

7:                                                ; preds = %3
  %8 = load i64, ptr %2, align 8
  %9 = lshr i64 %8, 33
  %10 = xor i64 %9, %8
  %11 = mul i64 %10, 1099511628211
  %12 = lshr i64 %11, 33
  %13 = xor i64 %12, %11
  %14 = mul i64 %13, 16777619
  %15 = lshr i64 %14, 33
  %.masked.i.i.i = and i64 %14, 9223372036854775807
  %16 = xor i64 %.masked.i.i.i, %15
  %17 = getelementptr i8, ptr %1, i64 16
  %18 = load i64, ptr %17, align 8
  %.not13 = icmp sgt i64 %18, 0
  br i1 %.not13, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %7
  %19 = load ptr, ptr %1, align 8
  br label %21

._crit_edge:                                      ; preds = %38, %7
  %20 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_1)
  tail call void @llvm.trap()
  unreachable

21:                                               ; preds = %.lr.ph, %38
  %.pn = phi i64 [ %16, %.lr.ph ], [ %39, %38 ]
  %.0215 = phi i64 [ 0, %.lr.ph ], [ %22, %38 ]
  %.0416 = srem i64 %.pn, %18
  %22 = add nuw nsw i64 %.0215, 1
  %23 = getelementptr %Entry, ptr %19, i64 %.0416
  %24 = load i8, ptr %23, align 1
  %25 = icmp eq i8 %24, 0
  br i1 %25, label %common.ret, label %26

26:                                               ; preds = %21
  %27 = getelementptr i8, ptr %23, i64 8
  %28 = load i64, ptr %27, align 8
  %29 = icmp eq i64 %28, %16
  br i1 %29, label %30, label %.thread

30:                                               ; preds = %26
  %31 = getelementptr i8, ptr %23, i64 16
  %32 = load i64, ptr %31, align 8
  %.not11 = icmp eq i64 %32, %8
  br i1 %.not11, label %common.ret, label %.thread

.thread:                                          ; preds = %26, %30
  %33 = add i64 %.0416, %18
  %34 = srem i64 %28, %18
  %35 = sub i64 %33, %34
  %36 = srem i64 %35, %18
  %37 = icmp slt i64 %36, %.0215
  br i1 %37, label %common.ret, label %38

38:                                               ; preds = %.thread
  %39 = add i64 %.0416, 1
  %.not = icmp slt i64 %22, %18
  br i1 %.not, label %21, label %._crit_edge

common.ret:                                       ; preds = %.thread, %30, %21, %3
  %storemerge = phi i8 [ 0, %3 ], [ 0, %21 ], [ 1, %30 ], [ 0, %.thread ]
  store i8 %storemerge, ptr %0, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = getelementptr i8, ptr %1, i64 8
  %5 = load i64, ptr %4, align 8
  %6 = icmp eq i64 %5, 0
  br i1 %6, label %7, label %9

7:                                                ; preds = %3
  %8 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_2)
  tail call void @llvm.trap()
  unreachable

9:                                                ; preds = %3
  %10 = load i64, ptr %2, align 8
  %11 = lshr i64 %10, 33
  %12 = xor i64 %11, %10
  %13 = mul i64 %12, 1099511628211
  %14 = lshr i64 %13, 33
  %15 = xor i64 %14, %13
  %16 = mul i64 %15, 16777619
  %17 = lshr i64 %16, 33
  %.masked.i.i.i = and i64 %16, 9223372036854775807
  %18 = xor i64 %.masked.i.i.i, %17
  %19 = getelementptr i8, ptr %1, i64 16
  %20 = load i64, ptr %19, align 8
  %.not14 = icmp sgt i64 %20, 0
  br i1 %.not14, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %9
  %21 = load ptr, ptr %1, align 8
  br label %23

._crit_edge:                                      ; preds = %42, %9
  %22 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_3)
  tail call void @llvm.trap()
  unreachable

23:                                               ; preds = %.lr.ph, %42
  %.pn = phi i64 [ %18, %.lr.ph ], [ %43, %42 ]
  %.0116 = phi i64 [ 0, %.lr.ph ], [ %24, %42 ]
  %.0317 = srem i64 %.pn, %20
  %24 = add nuw nsw i64 %.0116, 1
  %25 = getelementptr %Entry, ptr %21, i64 %.0317
  %26 = load i8, ptr %25, align 1
  %27 = icmp eq i8 %26, 0
  br i1 %27, label %47, label %28

28:                                               ; preds = %23
  %29 = getelementptr i8, ptr %25, i64 8
  %30 = load i64, ptr %29, align 8
  %31 = icmp eq i64 %30, %18
  br i1 %31, label %32, label %.thread

32:                                               ; preds = %28
  %33 = getelementptr i8, ptr %25, i64 16
  %34 = load i64, ptr %33, align 8
  %.not10 = icmp eq i64 %34, %10
  br i1 %.not10, label %44, label %.thread

.thread:                                          ; preds = %28, %32
  %35 = add i64 %.0317, %20
  %36 = srem i64 %30, %20
  %37 = sub i64 %35, %36
  %38 = srem i64 %37, %20
  %39 = icmp slt i64 %38, %.0116
  br i1 %39, label %40, label %42

40:                                               ; preds = %.thread
  %41 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_4)
  tail call void @llvm.trap()
  unreachable

42:                                               ; preds = %.thread
  %43 = add i64 %.0317, 1
  %.not = icmp slt i64 %24, %20
  br i1 %.not, label %23, label %._crit_edge

44:                                               ; preds = %32
  %45 = getelementptr i8, ptr %25, i64 24
  %46 = load i64, ptr %45, align 8
  store i64 %46, ptr %0, align 1
  ret void

47:                                               ; preds = %23
  %48 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_5)
  tail call void @llvm.trap()
  unreachable
}

; Function Attrs: nounwind
define internal fastcc void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr nocapture %0, ptr nocapture readonly %1, ptr nocapture readonly %2, ptr nocapture readonly %3) unnamed_addr #5 {
  %5 = getelementptr i8, ptr %0, i64 16
  %6 = load i64, ptr %5, align 8
  %.not33 = icmp sgt i64 %6, 0
  br i1 %.not33, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %4
  %7 = load i64, ptr %3, align 8
  %8 = load i64, ptr %2, align 8
  %9 = lshr i64 %8, 33
  %10 = xor i64 %9, %8
  %11 = mul i64 %10, 1099511628211
  %12 = lshr i64 %11, 33
  %13 = xor i64 %12, %11
  %14 = mul i64 %13, 16777619
  %.masked.i.i.i = and i64 %14, 9223372036854775807
  %15 = lshr i64 %14, 33
  %16 = xor i64 %.masked.i.i.i, %15
  %17 = urem i64 %16, %6
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %49
  %18 = phi i64 [ %50, %49 ], [ %6, %.lr.ph.preheader ]
  %.039 = phi i64 [ %53, %49 ], [ %17, %.lr.ph.preheader ]
  %.0238 = phi i64 [ %.1, %49 ], [ %16, %.lr.ph.preheader ]
  %.0337 = phi i64 [ %.14, %49 ], [ %7, %.lr.ph.preheader ]
  %.0536 = phi i64 [ %19, %49 ], [ 0, %.lr.ph.preheader ]
  %.0635 = phi i64 [ %51, %49 ], [ 0, %.lr.ph.preheader ]
  %.01634 = phi i64 [ %.117, %49 ], [ %8, %.lr.ph.preheader ]
  %19 = add nuw nsw i64 %.0536, 1
  %20 = load ptr, ptr %1, align 8
  %21 = getelementptr %Entry, ptr %20, i64 %.039
  %22 = load i8, ptr %21, align 1
  %23 = icmp eq i8 %22, 0
  %24 = getelementptr i8, ptr %21, i64 8
  br i1 %23, label %64, label %25

25:                                               ; preds = %.lr.ph
  %26 = load i64, ptr %24, align 8
  %27 = icmp eq i64 %26, %.0238
  br i1 %27, label %28, label %.thread

28:                                               ; preds = %25
  %29 = getelementptr i8, ptr %21, i64 16
  %30 = load i64, ptr %29, align 8
  %.not20 = icmp eq i64 %30, %.01634
  br i1 %.not20, label %54, label %.thread

.thread:                                          ; preds = %25, %28
  %31 = add i64 %18, %.039
  %32 = srem i64 %26, %18
  %33 = sub i64 %31, %32
  %34 = srem i64 %33, %18
  %35 = icmp slt i64 %34, %.0635
  br i1 %35, label %36, label %49

36:                                               ; preds = %.thread
  %37 = getelementptr i8, ptr %21, i64 16
  %38 = load i64, ptr %37, align 8
  %39 = getelementptr i8, ptr %21, i64 24
  %40 = load i64, ptr %39, align 8
  store i64 %.0238, ptr %24, align 8
  store i64 %.01634, ptr %37, align 8
  store i64 %.0337, ptr %39, align 8
  %41 = load ptr, ptr %1, align 8
  %42 = getelementptr %Entry, ptr %41, i64 %.039
  store i8 %22, ptr %42, align 1
  %43 = getelementptr i8, ptr %42, i64 8
  %44 = load i64, ptr %24, align 8
  store i64 %44, ptr %43, align 8
  %45 = getelementptr i8, ptr %42, i64 16
  %46 = load i64, ptr %37, align 8
  store i64 %46, ptr %45, align 8
  %47 = getelementptr i8, ptr %42, i64 24
  %48 = load i64, ptr %39, align 8
  store i64 %48, ptr %47, align 8
  %.pre = load i64, ptr %5, align 8
  br label %49

49:                                               ; preds = %.thread, %36
  %50 = phi i64 [ %.pre, %36 ], [ %18, %.thread ]
  %.117 = phi i64 [ %38, %36 ], [ %.01634, %.thread ]
  %.17 = phi i64 [ %34, %36 ], [ %.0635, %.thread ]
  %.14 = phi i64 [ %40, %36 ], [ %.0337, %.thread ]
  %.1 = phi i64 [ %26, %36 ], [ %.0238, %.thread ]
  %51 = add nsw i64 %.17, 1
  %52 = add i64 %.039, 1
  %53 = srem i64 %52, %50
  %.not = icmp slt i64 %19, %50
  br i1 %.not, label %.lr.ph, label %._crit_edge

common.ret:                                       ; preds = %64, %54
  ret void

54:                                               ; preds = %28
  %55 = getelementptr i8, ptr %21, i64 24
  store i64 %.0337, ptr %55, align 8
  %56 = load ptr, ptr %1, align 8
  %57 = getelementptr %Entry, ptr %56, i64 %.039
  store i8 %22, ptr %57, align 1
  %58 = getelementptr i8, ptr %57, i64 8
  %59 = load i64, ptr %24, align 8
  store i64 %59, ptr %58, align 8
  %60 = getelementptr i8, ptr %57, i64 16
  %61 = load i64, ptr %29, align 8
  store i64 %61, ptr %60, align 8
  %62 = getelementptr i8, ptr %57, i64 24
  %63 = load i64, ptr %55, align 8
  store i64 %63, ptr %62, align 8
  br label %common.ret

64:                                               ; preds = %.lr.ph
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %24, i8 0, i64 24, i1 false)
  %65 = load ptr, ptr %1, align 8
  %66 = getelementptr %Entry, ptr %65, i64 %.039
  %67 = getelementptr i8, ptr %66, i64 8
  %68 = getelementptr i8, ptr %66, i64 16
  %69 = getelementptr i8, ptr %66, i64 24
  store i8 1, ptr %66, align 1
  store i64 %.0238, ptr %67, align 8
  store i64 %.01634, ptr %68, align 8
  store i64 %.0337, ptr %69, align 8
  %70 = getelementptr i8, ptr %0, i64 8
  %71 = load i64, ptr %70, align 8
  %72 = add i64 %71, 1
  store i64 %72, ptr %70, align 8
  br label %common.ret

._crit_edge:                                      ; preds = %49, %4
  %73 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_6)
  tail call void @llvm.trap()
  unreachable
}

; Function Attrs: nounwind
define void @rl_m_insert__DictTint64_tTint64_tT_int64_t_int64_t_r_bool(ptr nocapture writeonly %0, ptr nocapture %1, ptr nocapture readonly %2, ptr nocapture readonly %3) local_unnamed_addr #5 {
  %5 = getelementptr i8, ptr %1, i64 8
  %6 = load i64, ptr %5, align 8
  %7 = add i64 %6, 1
  %8 = sitofp i64 %7 to double
  %9 = getelementptr i8, ptr %1, i64 16
  %10 = load i64, ptr %9, align 8
  %11 = sitofp i64 %10 to double
  %12 = fdiv double %8, %11
  %13 = getelementptr i8, ptr %1, i64 24
  %14 = load double, ptr %13, align 8
  %15 = fcmp ogt double %12, %14
  br i1 %15, label %16, label %39

16:                                               ; preds = %4
  %17 = load ptr, ptr %1, align 8
  %18 = add i64 %10, 1
  br label %19

19:                                               ; preds = %19, %16
  %.0.i.i = phi i64 [ 1, %16 ], [ %21, %19 ]
  %20 = icmp slt i64 %.0.i.i, %18
  %21 = shl i64 %.0.i.i, 1
  br i1 %20, label %19, label %rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i

rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i: ; preds = %19
  store i64 %.0.i.i, ptr %9, align 8
  %22 = shl i64 %.0.i.i, 5
  %23 = tail call ptr @malloc(i64 %22)
  store ptr %23, ptr %1, align 8
  store i64 0, ptr %5, align 8
  %24 = icmp sgt i64 %.0.i.i, 0
  br i1 %24, label %.lr.ph.i, label %.preheader2.i

.preheader2.i:                                    ; preds = %.lr.ph.i, %rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i
  %25 = icmp sgt i64 %10, 0
  br i1 %25, label %.lr.ph5.i, label %rl_m__grow__DictTint64_tTint64_tT.exit

.lr.ph.i:                                         ; preds = %rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i, %.lr.ph.i
  %.03.i = phi i64 [ %28, %.lr.ph.i ], [ 0, %rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i ]
  %26 = load ptr, ptr %1, align 8
  %27 = getelementptr %Entry, ptr %26, i64 %.03.i
  store i8 0, ptr %27, align 1
  %28 = add nuw nsw i64 %.03.i, 1
  %29 = load i64, ptr %9, align 8
  %30 = icmp slt i64 %28, %29
  br i1 %30, label %.lr.ph.i, label %.preheader2.i

.lr.ph5.i:                                        ; preds = %.preheader2.i, %36
  %.14.i = phi i64 [ %37, %36 ], [ 0, %.preheader2.i ]
  %31 = getelementptr %Entry, ptr %17, i64 %.14.i
  %32 = load i8, ptr %31, align 1
  %.not.i = icmp eq i8 %32, 0
  br i1 %.not.i, label %36, label %33

33:                                               ; preds = %.lr.ph5.i
  %34 = getelementptr i8, ptr %31, i64 16
  %35 = getelementptr i8, ptr %31, i64 24
  tail call fastcc void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr nonnull %1, ptr nonnull %1, ptr %34, ptr %35)
  br label %36

36:                                               ; preds = %33, %.lr.ph5.i
  %37 = add nuw nsw i64 %.14.i, 1
  %38 = icmp slt i64 %37, %10
  br i1 %38, label %.lr.ph5.i, label %rl_m__grow__DictTint64_tTint64_tT.exit

rl_m__grow__DictTint64_tTint64_tT.exit:           ; preds = %36, %.preheader2.i
  tail call void @free(ptr %17)
  br label %39

39:                                               ; preds = %4, %rl_m__grow__DictTint64_tTint64_tT.exit
  tail call fastcc void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr %1, ptr %1, ptr %2, ptr %3)
  store i8 1, ptr %0, align 1
  ret void
}

; Function Attrs: nofree nounwind memory(write, argmem: readwrite, inaccessiblemem: readwrite)
define void @rl_m_init__DictTint64_tTint64_tT(ptr nocapture %0) local_unnamed_addr #7 {
.lr.ph.preheader:
  %1 = getelementptr i8, ptr %0, i64 16
  store i64 4, ptr %1, align 8
  %2 = getelementptr i8, ptr %0, i64 8
  store i64 0, ptr %2, align 8
  %3 = getelementptr i8, ptr %0, i64 24
  store double 7.500000e-01, ptr %3, align 8
  %4 = tail call dereferenceable_or_null(128) ptr @malloc(i64 128)
  store ptr %4, ptr %0, align 8
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %.01 = phi i64 [ %7, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %5 = load ptr, ptr %0, align 8
  %6 = getelementptr %Entry, ptr %5, i64 %.01
  store i8 0, ptr %6, align 1
  %7 = add nuw nsw i64 %.01, 1
  %8 = load i64, ptr %1, align 8
  %9 = icmp slt i64 %7, %8
  br i1 %9, label %.lr.ph, label %._crit_edge

._crit_edge:                                      ; preds = %.lr.ph
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write)
define void @rl_none__r_Nothing(ptr nocapture writeonly %0) local_unnamed_addr #3 {
  store i8 0, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_set_size__Range_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i64, ptr %1, align 8
  store i64 %3, ptr %0, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_size__Range_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i64, ptr %1, align 8
  store i64 %3, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_get__Range_int64_t_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readnone %1, ptr nocapture readonly %2) local_unnamed_addr #4 {
  %4 = load i64, ptr %2, align 8
  store i64 %4, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_range__int64_t_r_Range(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i64, ptr %1, align 8
  store i64 %3, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_size__VectorTint8_tT_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = getelementptr i8, ptr %1, i64 8
  %4 = load i64, ptr %3, align 8
  store i64 %4, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_size__VectorTint64_tT_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = getelementptr i8, ptr %1, i64 8
  %4 = load i64, ptr %3, align 8
  store i64 %4, ptr %0, align 1
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(write, argmem: readwrite, inaccessiblemem: none)
define void @rl_m_drop_back__VectorTint8_tT_int64_t(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #8 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = load i64, ptr %1, align 8
  %6 = sub i64 %4, %5
  %7 = icmp slt i64 %6, %4
  br i1 %7, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %2, %.lr.ph
  %.02 = phi i64 [ %10, %.lr.ph ], [ %6, %2 ]
  %8 = load ptr, ptr %0, align 8
  %9 = getelementptr i8, ptr %8, i64 %.02
  store i8 0, ptr %9, align 1
  %10 = add nsw i64 %.02, 1
  %11 = load i64, ptr %3, align 8
  %12 = icmp slt i64 %10, %11
  br i1 %12, label %.lr.ph, label %._crit_edge.loopexit

._crit_edge.loopexit:                             ; preds = %.lr.ph
  %.pre = load i64, ptr %1, align 8
  %.pre4 = sub i64 %11, %.pre
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %2
  %.pre-phi = phi i64 [ %.pre4, %._crit_edge.loopexit ], [ %6, %2 ]
  store i64 %.pre-phi, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_pop__VectorTint8_tT_r_int8_t(ptr nocapture writeonly %0, ptr nocapture %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %1, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %4, 0
  br i1 %5, label %8, label %6

6:                                                ; preds = %2
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_7)
  tail call void @llvm.trap()
  unreachable

8:                                                ; preds = %2
  %9 = add nsw i64 %4, -1
  %10 = load ptr, ptr %1, align 8
  %11 = getelementptr i8, ptr %10, i64 %9
  %12 = load i8, ptr %11, align 1
  store i64 %9, ptr %3, align 8
  store i8 0, ptr %11, align 1
  store i8 %12, ptr %0, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_append__VectorTint8_tT_int8_t(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = add i64 %4, 1
  %6 = getelementptr i8, ptr %0, i64 16
  %7 = load i64, ptr %6, align 8
  %8 = icmp sgt i64 %7, %5
  br i1 %8, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge, label %9

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge: ; preds = %2
  %.pre2 = load ptr, ptr %0, align 8
  br label %rl_m__grow__VectorTint8_tT_int64_t.exit

9:                                                ; preds = %2
  %10 = shl i64 %5, 1
  %11 = tail call ptr @malloc(i64 %10)
  %12 = ptrtoint ptr %11 to i64
  %13 = icmp sgt i64 %10, 0
  br i1 %13, label %.lr.ph.preheader.i, label %.preheader2.i

.lr.ph.preheader.i:                               ; preds = %9
  tail call void @llvm.memset.p0.i64(ptr align 1 %11, i8 0, i64 %10, i1 false)
  br label %.preheader2.i

.preheader2.i:                                    ; preds = %.lr.ph.preheader.i, %9
  %14 = icmp sgt i64 %4, 0
  %.pre.i = load ptr, ptr %0, align 8
  br i1 %14, label %.lr.ph5.i.preheader, label %.preheader.i

.lr.ph5.i.preheader:                              ; preds = %.preheader2.i
  %.pre.i3 = ptrtoint ptr %.pre.i to i64
  %min.iters.check = icmp ult i64 %4, 4
  %15 = sub i64 %12, %.pre.i3
  %diff.check = icmp ult i64 %15, 4
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph5.i.preheader4, label %vector.ph

.lr.ph5.i.preheader4:                             ; preds = %middle.block, %.lr.ph5.i.preheader
  %.14.i.ph = phi i64 [ 0, %.lr.ph5.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph5.i

vector.ph:                                        ; preds = %.lr.ph5.i.preheader
  %n.vec = and i64 %4, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %16 = getelementptr i8, ptr %11, i64 %index
  %17 = getelementptr i8, ptr %.pre.i, i64 %index
  %wide.load = load <4 x i8>, ptr %17, align 1
  store <4 x i8> %wide.load, ptr %16, align 1
  %index.next = add nuw i64 %index, 4
  %18 = icmp eq i64 %index.next, %n.vec
  br i1 %18, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %4, %n.vec
  br i1 %cmp.n, label %.preheader.i, label %.lr.ph5.i.preheader4

.preheader.i:                                     ; preds = %.lr.ph5.i, %middle.block, %.preheader2.i
  tail call void @free(ptr %.pre.i)
  store i64 %10, ptr %6, align 8
  store ptr %11, ptr %0, align 8
  %.pre = load i64, ptr %3, align 8
  br label %rl_m__grow__VectorTint8_tT_int64_t.exit

.lr.ph5.i:                                        ; preds = %.lr.ph5.i.preheader4, %.lr.ph5.i
  %.14.i = phi i64 [ %22, %.lr.ph5.i ], [ %.14.i.ph, %.lr.ph5.i.preheader4 ]
  %19 = getelementptr i8, ptr %11, i64 %.14.i
  %20 = getelementptr i8, ptr %.pre.i, i64 %.14.i
  %21 = load i8, ptr %20, align 1
  store i8 %21, ptr %19, align 1
  %22 = add nuw nsw i64 %.14.i, 1
  %23 = icmp slt i64 %22, %4
  br i1 %23, label %.lr.ph5.i, label %.preheader.i, !llvm.loop !4

rl_m__grow__VectorTint8_tT_int64_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge, %.preheader.i
  %24 = phi ptr [ %.pre2, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge ], [ %11, %.preheader.i ]
  %25 = phi i64 [ %4, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge ], [ %.pre, %.preheader.i ]
  %26 = getelementptr i8, ptr %24, i64 %25
  %27 = load i8, ptr %1, align 1
  store i8 %27, ptr %26, align 1
  %28 = load i64, ptr %3, align 8
  %29 = add i64 %28, 1
  store i64 %29, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_append__VectorTint64_tT_int64_t(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = add i64 %4, 1
  %6 = getelementptr i8, ptr %0, i64 16
  %7 = load i64, ptr %6, align 8
  %8 = icmp sgt i64 %7, %5
  br i1 %8, label %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge, label %9

.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge: ; preds = %2
  %.pre2 = load ptr, ptr %0, align 8
  br label %rl_m__grow__VectorTint64_tT_int64_t.exit

9:                                                ; preds = %2
  %10 = shl i64 %5, 4
  %11 = tail call ptr @malloc(i64 %10)
  %12 = trunc i64 %5 to i63
  %13 = icmp sgt i63 %12, 0
  br i1 %13, label %.lr.ph.preheader.i, label %.preheader2.i

.lr.ph.preheader.i:                               ; preds = %9
  tail call void @llvm.memset.p0.i64(ptr align 8 %11, i8 0, i64 %10, i1 false)
  br label %.preheader2.i

.preheader2.i:                                    ; preds = %.lr.ph.preheader.i, %9
  %14 = icmp sgt i64 %4, 0
  %.pre.i = load ptr, ptr %0, align 8
  br i1 %14, label %.lr.ph5.i, label %.preheader.i

.preheader.i:                                     ; preds = %.lr.ph5.i, %.preheader2.i
  tail call void @free(ptr %.pre.i)
  %15 = shl i64 %5, 1
  store i64 %15, ptr %6, align 8
  store ptr %11, ptr %0, align 8
  %.pre = load i64, ptr %3, align 8
  br label %rl_m__grow__VectorTint64_tT_int64_t.exit

.lr.ph5.i:                                        ; preds = %.preheader2.i, %.lr.ph5.i
  %.14.i = phi i64 [ %19, %.lr.ph5.i ], [ 0, %.preheader2.i ]
  %16 = getelementptr i64, ptr %11, i64 %.14.i
  %17 = getelementptr i64, ptr %.pre.i, i64 %.14.i
  %18 = load i64, ptr %17, align 8
  store i64 %18, ptr %16, align 8
  %19 = add nuw nsw i64 %.14.i, 1
  %20 = icmp slt i64 %19, %4
  br i1 %20, label %.lr.ph5.i, label %.preheader.i

rl_m__grow__VectorTint64_tT_int64_t.exit:         ; preds = %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge, %.preheader.i
  %21 = phi ptr [ %.pre2, %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge ], [ %11, %.preheader.i ]
  %22 = phi i64 [ %4, %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge ], [ %.pre, %.preheader.i ]
  %23 = getelementptr i64, ptr %21, i64 %22
  %24 = load i64, ptr %1, align 8
  store i64 %24, ptr %23, align 8
  %25 = load i64, ptr %3, align 8
  %26 = add i64 %25, 1
  store i64 %26, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = load i64, ptr %2, align 8
  %5 = icmp sgt i64 %4, -1
  br i1 %5, label %8, label %6

6:                                                ; preds = %3
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

8:                                                ; preds = %3
  %9 = getelementptr i8, ptr %1, i64 8
  %10 = load i64, ptr %9, align 8
  %11 = icmp slt i64 %4, %10
  br i1 %11, label %14, label %12

12:                                               ; preds = %8
  %13 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

14:                                               ; preds = %8
  %15 = load ptr, ptr %1, align 8
  %16 = getelementptr i8, ptr %15, i64 %4
  store ptr %16, ptr %0, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = load i64, ptr %2, align 8
  %5 = icmp sgt i64 %4, -1
  br i1 %5, label %8, label %6

6:                                                ; preds = %3
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

8:                                                ; preds = %3
  %9 = getelementptr i8, ptr %1, i64 8
  %10 = load i64, ptr %9, align 8
  %11 = icmp slt i64 %4, %10
  br i1 %11, label %14, label %12

12:                                               ; preds = %8
  %13 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

14:                                               ; preds = %8
  %15 = load ptr, ptr %1, align 8
  %16 = getelementptr i64, ptr %15, i64 %4
  store ptr %16, ptr %0, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_back__VectorTint8_tT_r_int8_tRef(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %1, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %4, 0
  br i1 %5, label %8, label %6

6:                                                ; preds = %2
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

8:                                                ; preds = %2
  %9 = load ptr, ptr %1, align 8
  %10 = getelementptr i8, ptr %9, i64 %4
  %11 = getelementptr i8, ptr %10, i64 -1
  store ptr %11, ptr %0, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 16
  %4 = load i64, ptr %3, align 8
  %.not2.i = icmp eq i64 %4, 0
  br i1 %.not2.i, label %rl_m_drop__VectorTint8_tT.exit, label %5

5:                                                ; preds = %2
  %6 = load ptr, ptr %0, align 8
  tail call void @free(ptr %6)
  br label %rl_m_drop__VectorTint8_tT.exit

rl_m_drop__VectorTint8_tT.exit:                   ; preds = %2, %5
  %7 = getelementptr i8, ptr %0, i64 8
  store i64 0, ptr %7, align 8
  store i64 4, ptr %3, align 8
  %8 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %8, ptr %0, align 8
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i, %rl_m_drop__VectorTint8_tT.exit
  %.01.i = phi i64 [ %11, %.lr.ph.i ], [ 0, %rl_m_drop__VectorTint8_tT.exit ]
  %9 = load ptr, ptr %0, align 8
  %10 = getelementptr i8, ptr %9, i64 %.01.i
  store i8 0, ptr %10, align 1
  %11 = add nuw nsw i64 %.01.i, 1
  %12 = load i64, ptr %3, align 8
  %13 = icmp slt i64 %11, %12
  br i1 %13, label %.lr.ph.i, label %rl_m_init__VectorTint8_tT.exit.preheader

rl_m_init__VectorTint8_tT.exit.preheader:         ; preds = %.lr.ph.i
  %14 = getelementptr i8, ptr %1, i64 8
  %15 = load i64, ptr %14, align 8
  %16 = icmp sgt i64 %15, 0
  br i1 %16, label %.lr.ph.preheader, label %rl_m_init__VectorTint8_tT.exit._crit_edge

.lr.ph.preheader:                                 ; preds = %rl_m_init__VectorTint8_tT.exit.preheader
  %.pr = load i64, ptr %7, align 8
  br label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit

rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit: ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit, %.lr.ph.preheader
  %17 = phi i64 [ %43, %rl_m_append__VectorTint8_tT_int8_t.exit ], [ %.pr, %.lr.ph.preheader ]
  %storemerge2 = phi i64 [ %44, %rl_m_append__VectorTint8_tT_int8_t.exit ], [ 0, %.lr.ph.preheader ]
  %18 = load ptr, ptr %1, align 8
  %19 = getelementptr i8, ptr %18, i64 %storemerge2
  %20 = add i64 %17, 1
  %21 = load i64, ptr %3, align 8
  %22 = icmp sgt i64 %21, %20
  br i1 %22, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, label %23

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i: ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit
  %.pre2.i = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

23:                                               ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit
  %24 = shl i64 %20, 1
  %25 = tail call ptr @malloc(i64 %24)
  %26 = ptrtoint ptr %25 to i64
  %27 = icmp sgt i64 %24, 0
  br i1 %27, label %.lr.ph.preheader.i.i, label %.preheader2.i.i

.lr.ph.preheader.i.i:                             ; preds = %23
  tail call void @llvm.memset.p0.i64(ptr align 1 %25, i8 0, i64 %24, i1 false)
  br label %.preheader2.i.i

.preheader2.i.i:                                  ; preds = %.lr.ph.preheader.i.i, %23
  %28 = icmp sgt i64 %17, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %28, label %.lr.ph5.i.i.preheader, label %.preheader.i.i

.lr.ph5.i.i.preheader:                            ; preds = %.preheader2.i.i
  %.pre.i.i3 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %17, 4
  %29 = sub i64 %26, %.pre.i.i3
  %diff.check = icmp ult i64 %29, 4
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph5.i.i.preheader4, label %vector.ph

.lr.ph5.i.i.preheader4:                           ; preds = %middle.block, %.lr.ph5.i.i.preheader
  %.14.i.i.ph = phi i64 [ 0, %.lr.ph5.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph5.i.i

vector.ph:                                        ; preds = %.lr.ph5.i.i.preheader
  %n.vec = and i64 %17, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %30 = getelementptr i8, ptr %25, i64 %index
  %31 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %wide.load = load <4 x i8>, ptr %31, align 1
  store <4 x i8> %wide.load, ptr %30, align 1
  %index.next = add nuw i64 %index, 4
  %32 = icmp eq i64 %index.next, %n.vec
  br i1 %32, label %middle.block, label %vector.body, !llvm.loop !5

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %17, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph5.i.i.preheader4

.preheader.i.i:                                   ; preds = %.lr.ph5.i.i, %middle.block, %.preheader2.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %24, ptr %3, align 8
  store ptr %25, ptr %0, align 8
  %.pre.i = load i64, ptr %7, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph5.i.i:                                      ; preds = %.lr.ph5.i.i.preheader4, %.lr.ph5.i.i
  %.14.i.i = phi i64 [ %36, %.lr.ph5.i.i ], [ %.14.i.i.ph, %.lr.ph5.i.i.preheader4 ]
  %33 = getelementptr i8, ptr %25, i64 %.14.i.i
  %34 = getelementptr i8, ptr %.pre.i.i, i64 %.14.i.i
  %35 = load i8, ptr %34, align 1
  store i8 %35, ptr %33, align 1
  %36 = add nuw nsw i64 %.14.i.i, 1
  %37 = icmp slt i64 %36, %17
  br i1 %37, label %.lr.ph5.i.i, label %.preheader.i.i, !llvm.loop !6

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %38 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %25, %.preheader.i.i ]
  %39 = phi i64 [ %17, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %40 = getelementptr i8, ptr %38, i64 %39
  %41 = load i8, ptr %19, align 1
  store i8 %41, ptr %40, align 1
  %42 = load i64, ptr %7, align 8
  %43 = add i64 %42, 1
  store i64 %43, ptr %7, align 8
  %44 = add nuw nsw i64 %storemerge2, 1
  %45 = load i64, ptr %14, align 8
  %46 = icmp slt i64 %44, %45
  br i1 %46, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit, label %rl_m_init__VectorTint8_tT.exit._crit_edge

rl_m_init__VectorTint8_tT.exit._crit_edge:        ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit, %rl_m_init__VectorTint8_tT.exit.preheader
  ret void
}

; Function Attrs: nounwind
define void @rl_m_assign__VectorTint64_tT_VectorTint64_tT(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 16
  %4 = load i64, ptr %3, align 8
  %.not2.i = icmp eq i64 %4, 0
  br i1 %.not2.i, label %rl_m_drop__VectorTint64_tT.exit, label %5

5:                                                ; preds = %2
  %6 = load ptr, ptr %0, align 8
  tail call void @free(ptr %6)
  br label %rl_m_drop__VectorTint64_tT.exit

rl_m_drop__VectorTint64_tT.exit:                  ; preds = %2, %5
  %7 = getelementptr i8, ptr %0, i64 8
  store i64 0, ptr %7, align 8
  store i64 4, ptr %3, align 8
  %8 = tail call dereferenceable_or_null(32) ptr @malloc(i64 32)
  store ptr %8, ptr %0, align 8
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i, %rl_m_drop__VectorTint64_tT.exit
  %.01.i = phi i64 [ %11, %.lr.ph.i ], [ 0, %rl_m_drop__VectorTint64_tT.exit ]
  %9 = load ptr, ptr %0, align 8
  %10 = getelementptr i64, ptr %9, i64 %.01.i
  store i64 0, ptr %10, align 8
  %11 = add nuw nsw i64 %.01.i, 1
  %12 = load i64, ptr %3, align 8
  %13 = icmp slt i64 %11, %12
  br i1 %13, label %.lr.ph.i, label %rl_m_init__VectorTint64_tT.exit.preheader

rl_m_init__VectorTint64_tT.exit.preheader:        ; preds = %.lr.ph.i
  %14 = getelementptr i8, ptr %1, i64 8
  %15 = load i64, ptr %14, align 8
  %16 = icmp sgt i64 %15, 0
  br i1 %16, label %.lr.ph.preheader, label %rl_m_init__VectorTint64_tT.exit._crit_edge

.lr.ph.preheader:                                 ; preds = %rl_m_init__VectorTint64_tT.exit.preheader
  %.pr = load i64, ptr %7, align 8
  br label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit: ; preds = %rl_m_append__VectorTint64_tT_int64_t.exit, %.lr.ph.preheader
  %17 = phi i64 [ %40, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %.pr, %.lr.ph.preheader ]
  %storemerge2 = phi i64 [ %41, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ 0, %.lr.ph.preheader ]
  %18 = load ptr, ptr %1, align 8
  %19 = getelementptr i64, ptr %18, i64 %storemerge2
  %20 = add i64 %17, 1
  %21 = load i64, ptr %3, align 8
  %22 = icmp sgt i64 %21, %20
  br i1 %22, label %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge.i, label %23

.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge.i: ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit
  %.pre2.i = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint64_tT_int64_t.exit

23:                                               ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit
  %24 = shl i64 %20, 4
  %25 = tail call ptr @malloc(i64 %24)
  %26 = trunc i64 %20 to i63
  %27 = icmp sgt i63 %26, 0
  br i1 %27, label %.lr.ph.preheader.i.i, label %.preheader2.i.i

.lr.ph.preheader.i.i:                             ; preds = %23
  tail call void @llvm.memset.p0.i64(ptr align 8 %25, i8 0, i64 %24, i1 false)
  br label %.preheader2.i.i

.preheader2.i.i:                                  ; preds = %.lr.ph.preheader.i.i, %23
  %28 = icmp sgt i64 %17, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %28, label %.lr.ph5.i.i, label %.preheader.i.i

.preheader.i.i:                                   ; preds = %.lr.ph5.i.i, %.preheader2.i.i
  tail call void @free(ptr %.pre.i.i)
  %29 = shl i64 %20, 1
  store i64 %29, ptr %3, align 8
  store ptr %25, ptr %0, align 8
  %.pre.i = load i64, ptr %7, align 8
  br label %rl_m_append__VectorTint64_tT_int64_t.exit

.lr.ph5.i.i:                                      ; preds = %.preheader2.i.i, %.lr.ph5.i.i
  %.14.i.i = phi i64 [ %33, %.lr.ph5.i.i ], [ 0, %.preheader2.i.i ]
  %30 = getelementptr i64, ptr %25, i64 %.14.i.i
  %31 = getelementptr i64, ptr %.pre.i.i, i64 %.14.i.i
  %32 = load i64, ptr %31, align 8
  store i64 %32, ptr %30, align 8
  %33 = add nuw nsw i64 %.14.i.i, 1
  %34 = icmp slt i64 %33, %17
  br i1 %34, label %.lr.ph5.i.i, label %.preheader.i.i

rl_m_append__VectorTint64_tT_int64_t.exit:        ; preds = %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %35 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge.i ], [ %25, %.preheader.i.i ]
  %36 = phi i64 [ %17, %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %37 = getelementptr i64, ptr %35, i64 %36
  %38 = load i64, ptr %19, align 8
  store i64 %38, ptr %37, align 8
  %39 = load i64, ptr %7, align 8
  %40 = add i64 %39, 1
  store i64 %40, ptr %7, align 8
  %41 = add nuw nsw i64 %storemerge2, 1
  %42 = load i64, ptr %14, align 8
  %43 = icmp slt i64 %41, %42
  br i1 %43, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit, label %rl_m_init__VectorTint64_tT.exit._crit_edge

rl_m_init__VectorTint64_tT.exit._crit_edge:       ; preds = %rl_m_append__VectorTint64_tT_int64_t.exit, %rl_m_init__VectorTint64_tT.exit.preheader
  ret void
}

; Function Attrs: mustprogress nounwind willreturn
define void @rl_m_drop__VectorTint8_tT(ptr nocapture %0) local_unnamed_addr #6 {
  %2 = getelementptr i8, ptr %0, i64 16
  %3 = load i64, ptr %2, align 8
  %.not2 = icmp eq i64 %3, 0
  br i1 %.not2, label %6, label %4

4:                                                ; preds = %1
  %5 = load ptr, ptr %0, align 8
  tail call void @free(ptr %5)
  br label %6

6:                                                ; preds = %1, %4
  %7 = getelementptr i8, ptr %0, i64 8
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %7, i8 0, i64 16, i1 false)
  ret void
}

; Function Attrs: mustprogress nounwind willreturn
define void @rl_m_drop__VectorTint64_tT(ptr nocapture %0) local_unnamed_addr #6 {
  %2 = getelementptr i8, ptr %0, i64 16
  %3 = load i64, ptr %2, align 8
  %.not2 = icmp eq i64 %3, 0
  br i1 %.not2, label %6, label %4

4:                                                ; preds = %1
  %5 = load ptr, ptr %0, align 8
  tail call void @free(ptr %5)
  br label %6

6:                                                ; preds = %1, %4
  %7 = getelementptr i8, ptr %0, i64 8
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %7, i8 0, i64 16, i1 false)
  ret void
}

; Function Attrs: nofree nounwind memory(write, argmem: readwrite, inaccessiblemem: readwrite)
define void @rl_m_init__VectorTint8_tT(ptr nocapture %0) local_unnamed_addr #7 {
.lr.ph.preheader:
  %1 = getelementptr i8, ptr %0, i64 8
  store i64 0, ptr %1, align 8
  %2 = getelementptr i8, ptr %0, i64 16
  store i64 4, ptr %2, align 8
  %3 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %3, ptr %0, align 8
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %.01 = phi i64 [ %6, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %4 = load ptr, ptr %0, align 8
  %5 = getelementptr i8, ptr %4, i64 %.01
  store i8 0, ptr %5, align 1
  %6 = add nuw nsw i64 %.01, 1
  %7 = load i64, ptr %2, align 8
  %8 = icmp slt i64 %6, %7
  br i1 %8, label %.lr.ph, label %._crit_edge

._crit_edge:                                      ; preds = %.lr.ph
  ret void
}

; Function Attrs: nofree nounwind memory(write, argmem: readwrite, inaccessiblemem: readwrite)
define void @rl_m_init__VectorTint64_tT(ptr nocapture %0) local_unnamed_addr #7 {
.lr.ph.preheader:
  %1 = getelementptr i8, ptr %0, i64 8
  store i64 0, ptr %1, align 8
  %2 = getelementptr i8, ptr %0, i64 16
  store i64 4, ptr %2, align 8
  %3 = tail call dereferenceable_or_null(32) ptr @malloc(i64 32)
  store ptr %3, ptr %0, align 8
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %.01 = phi i64 [ %6, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %4 = load ptr, ptr %0, align 8
  %5 = getelementptr i64, ptr %4, i64 %.01
  store i64 0, ptr %5, align 8
  %6 = add nuw nsw i64 %.01, 1
  %7 = load i64, ptr %2, align 8
  %8 = icmp slt i64 %6, %7
  br i1 %8, label %.lr.ph, label %._crit_edge

._crit_edge:                                      ; preds = %.lr.ph
  ret void
}

; Function Attrs: nounwind
define void @rl_m_to_indented_lines__String_r_String(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
rl_m_init__String.exit:
  %2 = alloca ptr, align 8
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca %String, align 8
  %6 = alloca %String, align 8
  %7 = getelementptr inbounds i8, ptr %6, i64 8
  %8 = getelementptr inbounds i8, ptr %6, i64 16
  store i64 4, ptr %8, align 8
  %9 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %9, ptr %6, align 8
  store i32 0, ptr %9, align 1
  store i64 1, ptr %7, align 8
  %10 = getelementptr i8, ptr %1, i64 8
  %11 = load i64, ptr %10, align 8
  %12 = add i64 %11, -1
  %13 = icmp sgt i64 %12, 0
  br i1 %13, label %.lr.ph, label %.critedge

.lr.ph:                                           ; preds = %rl_m_init__String.exit, %233
  %14 = phi i64 [ %235, %233 ], [ %11, %rl_m_init__String.exit ]
  %.0149 = phi i64 [ %234, %233 ], [ 0, %rl_m_init__String.exit ]
  %.0140148 = phi i64 [ %.1141, %233 ], [ 0, %rl_m_init__String.exit ]
  %15 = icmp sgt i64 %.0149, -1
  br i1 %15, label %18, label %16

16:                                               ; preds = %.lr.ph
  %17 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

18:                                               ; preds = %.lr.ph
  %19 = icmp slt i64 %.0149, %14
  br i1 %19, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %20

20:                                               ; preds = %18
  %21 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %18
  %22 = load ptr, ptr %1, align 8
  %23 = getelementptr i8, ptr %22, i64 %.0149
  %24 = load i8, ptr %23, align 1
  switch i8 %24, label %rl_m_get__String_int64_t_r_int8_tRef.exit5 [
    i8 40, label %rl_m_get__String_int64_t_r_int8_tRef.exit67
    i8 91, label %rl_m_get__String_int64_t_r_int8_tRef.exit67
    i8 123, label %rl_m_get__String_int64_t_r_int8_tRef.exit67
    i8 41, label %rl_is_close_paren__int8_t_r_bool.exit.thread
    i8 125, label %rl_is_close_paren__int8_t_r_bool.exit.thread
    i8 93, label %rl_is_close_paren__int8_t_r_bool.exit.thread
    i8 44, label %rl_m_get__String_int64_t_r_int8_tRef.exit15
  ]

rl_m_get__String_int64_t_r_int8_tRef.exit5:       ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %25 = load i64, ptr %7, align 8
  %26 = icmp sgt i64 %25, 0
  br i1 %26, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i, label %27

27:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit5
  %28 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i:     ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit5
  %29 = load ptr, ptr %6, align 8
  %30 = ptrtoint ptr %29 to i64
  %31 = getelementptr i8, ptr %29, i64 %25
  %32 = getelementptr i8, ptr %31, i64 -1
  store i8 %24, ptr %32, align 1
  %33 = add nuw i64 %25, 1
  %34 = load i64, ptr %8, align 8
  %35 = icmp sgt i64 %34, %33
  br i1 %35, label %rl_m_append__String_int8_t.exit, label %36

36:                                               ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i
  %37 = shl i64 %33, 1
  %38 = tail call ptr @malloc(i64 %37)
  %39 = ptrtoint ptr %38 to i64
  %40 = icmp sgt i64 %37, 0
  br i1 %40, label %.lr.ph.preheader.i.i.i12, label %.lr.ph5.i.i.i10.preheader

.lr.ph.preheader.i.i.i12:                         ; preds = %36
  tail call void @llvm.memset.p0.i64(ptr align 1 %38, i8 0, i64 %37, i1 false)
  br label %.lr.ph5.i.i.i10.preheader

.lr.ph5.i.i.i10.preheader:                        ; preds = %36, %.lr.ph.preheader.i.i.i12
  %smax = tail call i64 @llvm.smax.i64(i64 %25, i64 1)
  %min.iters.check = icmp slt i64 %25, 4
  %41 = sub i64 %39, %30
  %diff.check = icmp ult i64 %41, 4
  %or.cond = or i1 %min.iters.check, %diff.check
  br i1 %or.cond, label %.lr.ph5.i.i.i10.preheader249, label %vector.ph

.lr.ph5.i.i.i10.preheader249:                     ; preds = %middle.block, %.lr.ph5.i.i.i10.preheader
  %.14.i.i.i11.ph = phi i64 [ 0, %.lr.ph5.i.i.i10.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph5.i.i.i10

vector.ph:                                        ; preds = %.lr.ph5.i.i.i10.preheader
  %n.vec = and i64 %smax, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %42 = getelementptr i8, ptr %38, i64 %index
  %43 = getelementptr i8, ptr %29, i64 %index
  %wide.load = load <4 x i8>, ptr %43, align 1
  store <4 x i8> %wide.load, ptr %42, align 1
  %index.next = add nuw i64 %index, 4
  %44 = icmp eq i64 %index.next, %n.vec
  br i1 %44, label %middle.block, label %vector.body, !llvm.loop !7

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %smax, %n.vec
  br i1 %cmp.n, label %.preheader.i.i.i8, label %.lr.ph5.i.i.i10.preheader249

.preheader.i.i.i8:                                ; preds = %.lr.ph5.i.i.i10, %middle.block
  tail call void @free(ptr nonnull %29)
  store i64 %37, ptr %8, align 8
  store ptr %38, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit

.lr.ph5.i.i.i10:                                  ; preds = %.lr.ph5.i.i.i10.preheader249, %.lr.ph5.i.i.i10
  %.14.i.i.i11 = phi i64 [ %48, %.lr.ph5.i.i.i10 ], [ %.14.i.i.i11.ph, %.lr.ph5.i.i.i10.preheader249 ]
  %45 = getelementptr i8, ptr %38, i64 %.14.i.i.i11
  %46 = getelementptr i8, ptr %29, i64 %.14.i.i.i11
  %47 = load i8, ptr %46, align 1
  store i8 %47, ptr %45, align 1
  %48 = add nuw nsw i64 %.14.i.i.i11, 1
  %49 = icmp slt i64 %48, %25
  br i1 %49, label %.lr.ph5.i.i.i10, label %.preheader.i.i.i8, !llvm.loop !8

rl_m_append__String_int8_t.exit:                  ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i, %.preheader.i.i.i8
  %50 = phi ptr [ %38, %.preheader.i.i.i8 ], [ %29, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i ]
  %51 = getelementptr i8, ptr %50, i64 %25
  store i8 0, ptr %51, align 1
  store i64 %33, ptr %7, align 8
  br label %233

rl_m_get__String_int64_t_r_int8_tRef.exit15:      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %52 = load i64, ptr %7, align 8
  %53 = icmp sgt i64 %52, 0
  br i1 %53, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16, label %54

54:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit15
  %55 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16:   ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit15
  %56 = load ptr, ptr %6, align 8
  %57 = ptrtoint ptr %56 to i64
  %58 = getelementptr i8, ptr %56, i64 %52
  %59 = getelementptr i8, ptr %58, i64 -1
  store i8 44, ptr %59, align 1
  %60 = add nuw i64 %52, 1
  %61 = load i64, ptr %8, align 8
  %62 = icmp sgt i64 %61, %60
  br i1 %62, label %rl_m_append__String_int8_t.exit26, label %63

63:                                               ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16
  %64 = shl i64 %60, 1
  %65 = tail call ptr @malloc(i64 %64)
  %66 = ptrtoint ptr %65 to i64
  %67 = icmp sgt i64 %64, 0
  br i1 %67, label %.lr.ph.preheader.i.i.i23, label %.lr.ph5.i.i.i21.preheader

.lr.ph.preheader.i.i.i23:                         ; preds = %63
  tail call void @llvm.memset.p0.i64(ptr align 1 %65, i8 0, i64 %64, i1 false)
  br label %.lr.ph5.i.i.i21.preheader

.lr.ph5.i.i.i21.preheader:                        ; preds = %63, %.lr.ph.preheader.i.i.i23
  %smax230 = tail call i64 @llvm.smax.i64(i64 %52, i64 1)
  %min.iters.check233 = icmp slt i64 %52, 4
  %68 = sub i64 %66, %57
  %diff.check229 = icmp ult i64 %68, 4
  %or.cond243 = or i1 %min.iters.check233, %diff.check229
  br i1 %or.cond243, label %.lr.ph5.i.i.i21.preheader254, label %vector.ph234

.lr.ph5.i.i.i21.preheader254:                     ; preds = %middle.block231, %.lr.ph5.i.i.i21.preheader
  %.14.i.i.i22.ph = phi i64 [ 0, %.lr.ph5.i.i.i21.preheader ], [ %n.vec236, %middle.block231 ]
  br label %.lr.ph5.i.i.i21

vector.ph234:                                     ; preds = %.lr.ph5.i.i.i21.preheader
  %n.vec236 = and i64 %smax230, 9223372036854775804
  br label %vector.body238

vector.body238:                                   ; preds = %vector.body238, %vector.ph234
  %index239 = phi i64 [ 0, %vector.ph234 ], [ %index.next241, %vector.body238 ]
  %69 = getelementptr i8, ptr %65, i64 %index239
  %70 = getelementptr i8, ptr %56, i64 %index239
  %wide.load240 = load <4 x i8>, ptr %70, align 1
  store <4 x i8> %wide.load240, ptr %69, align 1
  %index.next241 = add nuw i64 %index239, 4
  %71 = icmp eq i64 %index.next241, %n.vec236
  br i1 %71, label %middle.block231, label %vector.body238, !llvm.loop !9

middle.block231:                                  ; preds = %vector.body238
  %cmp.n242 = icmp eq i64 %smax230, %n.vec236
  br i1 %cmp.n242, label %.preheader.i.i.i19, label %.lr.ph5.i.i.i21.preheader254

.preheader.i.i.i19:                               ; preds = %.lr.ph5.i.i.i21, %middle.block231
  tail call void @free(ptr nonnull %56)
  store i64 %64, ptr %8, align 8
  store ptr %65, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit26

.lr.ph5.i.i.i21:                                  ; preds = %.lr.ph5.i.i.i21.preheader254, %.lr.ph5.i.i.i21
  %.14.i.i.i22 = phi i64 [ %75, %.lr.ph5.i.i.i21 ], [ %.14.i.i.i22.ph, %.lr.ph5.i.i.i21.preheader254 ]
  %72 = getelementptr i8, ptr %65, i64 %.14.i.i.i22
  %73 = getelementptr i8, ptr %56, i64 %.14.i.i.i22
  %74 = load i8, ptr %73, align 1
  store i8 %74, ptr %72, align 1
  %75 = add nuw nsw i64 %.14.i.i.i22, 1
  %76 = icmp slt i64 %75, %52
  br i1 %76, label %.lr.ph5.i.i.i21, label %.preheader.i.i.i19, !llvm.loop !10

rl_m_append__String_int8_t.exit26:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16, %.preheader.i.i.i19
  %77 = phi i64 [ %64, %.preheader.i.i.i19 ], [ %61, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16 ]
  %.pre2.i.i36 = phi ptr [ %65, %.preheader.i.i.i19 ], [ %56, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16 ]
  %.pre2.i.i36213 = ptrtoint ptr %.pre2.i.i36 to i64
  %78 = getelementptr i8, ptr %.pre2.i.i36, i64 %52
  store i8 0, ptr %78, align 1
  store i64 %60, ptr %7, align 8
  %.not = icmp eq i64 %52, 9223372036854775807
  br i1 %.not, label %79, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27

79:                                               ; preds = %rl_m_append__String_int8_t.exit26
  %80 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27:   ; preds = %rl_m_append__String_int8_t.exit26
  %81 = getelementptr i8, ptr %.pre2.i.i36, i64 %60
  %82 = getelementptr i8, ptr %81, i64 -1
  store i8 10, ptr %82, align 1
  %83 = add nuw i64 %52, 2
  %84 = icmp sgt i64 %77, %83
  br i1 %84, label %rl_m_append__String_int8_t.exit37, label %85

85:                                               ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27
  %86 = shl i64 %83, 1
  %87 = tail call ptr @malloc(i64 %86)
  %88 = ptrtoint ptr %87 to i64
  %89 = icmp sgt i64 %86, 0
  br i1 %89, label %.lr.ph.preheader.i.i.i34, label %.lr.ph5.i.i.i32.preheader

.lr.ph.preheader.i.i.i34:                         ; preds = %85
  tail call void @llvm.memset.p0.i64(ptr align 1 %87, i8 0, i64 %86, i1 false)
  br label %.lr.ph5.i.i.i32.preheader

.lr.ph5.i.i.i32.preheader:                        ; preds = %85, %.lr.ph.preheader.i.i.i34
  %smax215 = tail call i64 @llvm.smax.i64(i64 %60, i64 1)
  %min.iters.check218 = icmp slt i64 %60, 4
  %90 = sub i64 %88, %.pre2.i.i36213
  %diff.check214 = icmp ult i64 %90, 4
  %or.cond244 = or i1 %min.iters.check218, %diff.check214
  br i1 %or.cond244, label %.lr.ph5.i.i.i32.preheader253, label %vector.ph219

.lr.ph5.i.i.i32.preheader253:                     ; preds = %middle.block216, %.lr.ph5.i.i.i32.preheader
  %.14.i.i.i33.ph = phi i64 [ 0, %.lr.ph5.i.i.i32.preheader ], [ %n.vec221, %middle.block216 ]
  br label %.lr.ph5.i.i.i32

vector.ph219:                                     ; preds = %.lr.ph5.i.i.i32.preheader
  %n.vec221 = and i64 %smax215, 9223372036854775804
  br label %vector.body223

vector.body223:                                   ; preds = %vector.body223, %vector.ph219
  %index224 = phi i64 [ 0, %vector.ph219 ], [ %index.next226, %vector.body223 ]
  %91 = getelementptr i8, ptr %87, i64 %index224
  %92 = getelementptr i8, ptr %.pre2.i.i36, i64 %index224
  %wide.load225 = load <4 x i8>, ptr %92, align 1
  store <4 x i8> %wide.load225, ptr %91, align 1
  %index.next226 = add nuw i64 %index224, 4
  %93 = icmp eq i64 %index.next226, %n.vec221
  br i1 %93, label %middle.block216, label %vector.body223, !llvm.loop !11

middle.block216:                                  ; preds = %vector.body223
  %cmp.n227 = icmp eq i64 %smax215, %n.vec221
  br i1 %cmp.n227, label %.preheader.i.i.i30, label %.lr.ph5.i.i.i32.preheader253

.preheader.i.i.i30:                               ; preds = %.lr.ph5.i.i.i32, %middle.block216
  tail call void @free(ptr nonnull %.pre2.i.i36)
  store i64 %86, ptr %8, align 8
  store ptr %87, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit37

.lr.ph5.i.i.i32:                                  ; preds = %.lr.ph5.i.i.i32.preheader253, %.lr.ph5.i.i.i32
  %.14.i.i.i33 = phi i64 [ %97, %.lr.ph5.i.i.i32 ], [ %.14.i.i.i33.ph, %.lr.ph5.i.i.i32.preheader253 ]
  %94 = getelementptr i8, ptr %87, i64 %.14.i.i.i33
  %95 = getelementptr i8, ptr %.pre2.i.i36, i64 %.14.i.i.i33
  %96 = load i8, ptr %95, align 1
  store i8 %96, ptr %94, align 1
  %97 = add nuw nsw i64 %.14.i.i.i33, 1
  %98 = icmp slt i64 %97, %60
  br i1 %98, label %.lr.ph5.i.i.i32, label %.preheader.i.i.i30, !llvm.loop !12

rl_m_append__String_int8_t.exit37:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27, %.preheader.i.i.i30
  %99 = phi ptr [ %87, %.preheader.i.i.i30 ], [ %.pre2.i.i36, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27 ]
  %100 = getelementptr i8, ptr %99, i64 %60
  store i8 0, ptr %100, align 1
  %101 = load i64, ptr %7, align 8
  %102 = add i64 %101, 1
  store i64 %102, ptr %7, align 8
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %4)
  %.not1.i = icmp eq i64 %.0140148, 0
  br i1 %.not1.i, label %rl__indent_string__String_int64_t.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %rl_m_append__String_int8_t.exit37, %.lr.ph.i
  %.02.i = phi i64 [ %103, %.lr.ph.i ], [ 0, %rl_m_append__String_int8_t.exit37 ]
  store ptr @str_11, ptr %4, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %6, ptr nonnull %4)
  %103 = add nuw i64 %.02.i, 1
  %.not.i = icmp eq i64 %103, %.0140148
  br i1 %.not.i, label %rl__indent_string__String_int64_t.exit, label %.lr.ph.i

rl__indent_string__String_int64_t.exit:           ; preds = %.lr.ph.i, %rl_m_append__String_int8_t.exit37
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %4)
  %104 = add nuw i64 %.0149, 1
  %105 = icmp sgt i64 %104, -1
  br i1 %105, label %108, label %106

106:                                              ; preds = %rl__indent_string__String_int64_t.exit
  %107 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

108:                                              ; preds = %rl__indent_string__String_int64_t.exit
  %109 = load i64, ptr %10, align 8
  %110 = icmp slt i64 %104, %109
  br i1 %110, label %rl_m_get__String_int64_t_r_int8_tRef.exit38, label %111

111:                                              ; preds = %108
  %112 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit38:      ; preds = %108
  %113 = load ptr, ptr %1, align 8
  %114 = getelementptr i8, ptr %113, i64 %104
  %115 = load i8, ptr %114, align 1
  %116 = icmp eq i8 %115, 32
  %spec.select = select i1 %116, i64 %104, i64 %.0149
  br label %233

rl_is_close_paren__int8_t_r_bool.exit.thread:     ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %117 = load i64, ptr %7, align 8
  %118 = icmp sgt i64 %117, 0
  br i1 %118, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39, label %119

119:                                              ; preds = %rl_is_close_paren__int8_t_r_bool.exit.thread
  %120 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39:   ; preds = %rl_is_close_paren__int8_t_r_bool.exit.thread
  %121 = load ptr, ptr %6, align 8
  %122 = ptrtoint ptr %121 to i64
  %123 = getelementptr i8, ptr %121, i64 %117
  %124 = getelementptr i8, ptr %123, i64 -1
  store i8 10, ptr %124, align 1
  %125 = add nuw i64 %117, 1
  %126 = load i64, ptr %8, align 8
  %127 = icmp sgt i64 %126, %125
  br i1 %127, label %rl_m_append__String_int8_t.exit49, label %128

128:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39
  %129 = shl i64 %125, 1
  %130 = tail call ptr @malloc(i64 %129)
  %131 = ptrtoint ptr %130 to i64
  %132 = icmp sgt i64 %129, 0
  br i1 %132, label %.lr.ph.preheader.i.i.i46, label %.lr.ph5.i.i.i44.preheader

.lr.ph.preheader.i.i.i46:                         ; preds = %128
  tail call void @llvm.memset.p0.i64(ptr align 1 %130, i8 0, i64 %129, i1 false)
  br label %.lr.ph5.i.i.i44.preheader

.lr.ph5.i.i.i44.preheader:                        ; preds = %128, %.lr.ph.preheader.i.i.i46
  %smax199 = tail call i64 @llvm.smax.i64(i64 %117, i64 1)
  %min.iters.check202 = icmp slt i64 %117, 4
  %133 = sub i64 %131, %122
  %diff.check198 = icmp ult i64 %133, 4
  %or.cond245 = or i1 %min.iters.check202, %diff.check198
  br i1 %or.cond245, label %.lr.ph5.i.i.i44.preheader252, label %vector.ph203

.lr.ph5.i.i.i44.preheader252:                     ; preds = %middle.block200, %.lr.ph5.i.i.i44.preheader
  %.14.i.i.i45.ph = phi i64 [ 0, %.lr.ph5.i.i.i44.preheader ], [ %n.vec205, %middle.block200 ]
  br label %.lr.ph5.i.i.i44

vector.ph203:                                     ; preds = %.lr.ph5.i.i.i44.preheader
  %n.vec205 = and i64 %smax199, 9223372036854775804
  br label %vector.body207

vector.body207:                                   ; preds = %vector.body207, %vector.ph203
  %index208 = phi i64 [ 0, %vector.ph203 ], [ %index.next210, %vector.body207 ]
  %134 = getelementptr i8, ptr %130, i64 %index208
  %135 = getelementptr i8, ptr %121, i64 %index208
  %wide.load209 = load <4 x i8>, ptr %135, align 1
  store <4 x i8> %wide.load209, ptr %134, align 1
  %index.next210 = add nuw i64 %index208, 4
  %136 = icmp eq i64 %index.next210, %n.vec205
  br i1 %136, label %middle.block200, label %vector.body207, !llvm.loop !13

middle.block200:                                  ; preds = %vector.body207
  %cmp.n211 = icmp eq i64 %smax199, %n.vec205
  br i1 %cmp.n211, label %.preheader.i.i.i42, label %.lr.ph5.i.i.i44.preheader252

.preheader.i.i.i42:                               ; preds = %.lr.ph5.i.i.i44, %middle.block200
  tail call void @free(ptr nonnull %121)
  store i64 %129, ptr %8, align 8
  store ptr %130, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit49

.lr.ph5.i.i.i44:                                  ; preds = %.lr.ph5.i.i.i44.preheader252, %.lr.ph5.i.i.i44
  %.14.i.i.i45 = phi i64 [ %140, %.lr.ph5.i.i.i44 ], [ %.14.i.i.i45.ph, %.lr.ph5.i.i.i44.preheader252 ]
  %137 = getelementptr i8, ptr %130, i64 %.14.i.i.i45
  %138 = getelementptr i8, ptr %121, i64 %.14.i.i.i45
  %139 = load i8, ptr %138, align 1
  store i8 %139, ptr %137, align 1
  %140 = add nuw nsw i64 %.14.i.i.i45, 1
  %141 = icmp slt i64 %140, %117
  br i1 %141, label %.lr.ph5.i.i.i44, label %.preheader.i.i.i42, !llvm.loop !14

rl_m_append__String_int8_t.exit49:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39, %.preheader.i.i.i42
  %142 = phi ptr [ %130, %.preheader.i.i.i42 ], [ %121, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39 ]
  %143 = getelementptr i8, ptr %142, i64 %117
  store i8 0, ptr %143, align 1
  store i64 %125, ptr %7, align 8
  %144 = add i64 %.0140148, -1
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %3)
  %.not1.i50 = icmp eq i64 %144, 0
  br i1 %.not1.i50, label %.loopexit, label %.lr.ph.i51

.lr.ph.i51:                                       ; preds = %rl_m_append__String_int8_t.exit49, %.lr.ph.i51
  %.02.i52 = phi i64 [ %145, %.lr.ph.i51 ], [ 0, %rl_m_append__String_int8_t.exit49 ]
  store ptr @str_11, ptr %3, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %6, ptr nonnull %3)
  %145 = add nuw i64 %.02.i52, 1
  %.not.i53 = icmp eq i64 %145, %144
  br i1 %.not.i53, label %.loopexit, label %.lr.ph.i51

.loopexit:                                        ; preds = %.lr.ph.i51, %rl_m_append__String_int8_t.exit49
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %3)
  %146 = load i64, ptr %10, align 8
  %147 = icmp slt i64 %.0149, %146
  br i1 %147, label %rl_m_get__String_int64_t_r_int8_tRef.exit55, label %148

148:                                              ; preds = %.loopexit
  %149 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit55:      ; preds = %.loopexit
  %150 = load i64, ptr %7, align 8
  %151 = icmp sgt i64 %150, 0
  br i1 %151, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56, label %152

152:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit55
  %153 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56:   ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit55
  %154 = load ptr, ptr %1, align 8
  %155 = getelementptr i8, ptr %154, i64 %.0149
  %156 = load ptr, ptr %6, align 8
  %157 = ptrtoint ptr %156 to i64
  %158 = getelementptr i8, ptr %156, i64 %150
  %159 = getelementptr i8, ptr %158, i64 -1
  %160 = load i8, ptr %155, align 1
  store i8 %160, ptr %159, align 1
  %161 = add nuw i64 %150, 1
  %162 = load i64, ptr %8, align 8
  %163 = icmp sgt i64 %162, %161
  br i1 %163, label %rl_m_append__String_int8_t.exit66, label %164

164:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56
  %165 = shl i64 %161, 1
  %166 = tail call ptr @malloc(i64 %165)
  %167 = ptrtoint ptr %166 to i64
  %168 = icmp sgt i64 %165, 0
  br i1 %168, label %.lr.ph.preheader.i.i.i63, label %.lr.ph5.i.i.i61.preheader

.lr.ph.preheader.i.i.i63:                         ; preds = %164
  tail call void @llvm.memset.p0.i64(ptr align 1 %166, i8 0, i64 %165, i1 false)
  br label %.lr.ph5.i.i.i61.preheader

.lr.ph5.i.i.i61.preheader:                        ; preds = %164, %.lr.ph.preheader.i.i.i63
  %smax184 = tail call i64 @llvm.smax.i64(i64 %150, i64 1)
  %min.iters.check187 = icmp slt i64 %150, 4
  %169 = sub i64 %167, %157
  %diff.check183 = icmp ult i64 %169, 4
  %or.cond246 = or i1 %min.iters.check187, %diff.check183
  br i1 %or.cond246, label %.lr.ph5.i.i.i61.preheader251, label %vector.ph188

.lr.ph5.i.i.i61.preheader251:                     ; preds = %middle.block185, %.lr.ph5.i.i.i61.preheader
  %.14.i.i.i62.ph = phi i64 [ 0, %.lr.ph5.i.i.i61.preheader ], [ %n.vec190, %middle.block185 ]
  br label %.lr.ph5.i.i.i61

vector.ph188:                                     ; preds = %.lr.ph5.i.i.i61.preheader
  %n.vec190 = and i64 %smax184, 9223372036854775804
  br label %vector.body192

vector.body192:                                   ; preds = %vector.body192, %vector.ph188
  %index193 = phi i64 [ 0, %vector.ph188 ], [ %index.next195, %vector.body192 ]
  %170 = getelementptr i8, ptr %166, i64 %index193
  %171 = getelementptr i8, ptr %156, i64 %index193
  %wide.load194 = load <4 x i8>, ptr %171, align 1
  store <4 x i8> %wide.load194, ptr %170, align 1
  %index.next195 = add nuw i64 %index193, 4
  %172 = icmp eq i64 %index.next195, %n.vec190
  br i1 %172, label %middle.block185, label %vector.body192, !llvm.loop !15

middle.block185:                                  ; preds = %vector.body192
  %cmp.n196 = icmp eq i64 %smax184, %n.vec190
  br i1 %cmp.n196, label %.preheader.i.i.i59, label %.lr.ph5.i.i.i61.preheader251

.preheader.i.i.i59:                               ; preds = %.lr.ph5.i.i.i61, %middle.block185
  tail call void @free(ptr nonnull %156)
  store i64 %165, ptr %8, align 8
  store ptr %166, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit66

.lr.ph5.i.i.i61:                                  ; preds = %.lr.ph5.i.i.i61.preheader251, %.lr.ph5.i.i.i61
  %.14.i.i.i62 = phi i64 [ %176, %.lr.ph5.i.i.i61 ], [ %.14.i.i.i62.ph, %.lr.ph5.i.i.i61.preheader251 ]
  %173 = getelementptr i8, ptr %166, i64 %.14.i.i.i62
  %174 = getelementptr i8, ptr %156, i64 %.14.i.i.i62
  %175 = load i8, ptr %174, align 1
  store i8 %175, ptr %173, align 1
  %176 = add nuw nsw i64 %.14.i.i.i62, 1
  %177 = icmp slt i64 %176, %150
  br i1 %177, label %.lr.ph5.i.i.i61, label %.preheader.i.i.i59, !llvm.loop !16

rl_m_append__String_int8_t.exit66:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56, %.preheader.i.i.i59
  %178 = phi ptr [ %166, %.preheader.i.i.i59 ], [ %156, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56 ]
  %179 = getelementptr i8, ptr %178, i64 %150
  store i8 0, ptr %179, align 1
  store i64 %161, ptr %7, align 8
  br label %233

rl_m_get__String_int64_t_r_int8_tRef.exit67:      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %180 = load i64, ptr %7, align 8
  %181 = icmp sgt i64 %180, 0
  br i1 %181, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68, label %182

182:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit67
  %183 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68:   ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit67
  %184 = load ptr, ptr %6, align 8
  %185 = ptrtoint ptr %184 to i64
  %186 = getelementptr i8, ptr %184, i64 %180
  %187 = getelementptr i8, ptr %186, i64 -1
  store i8 %24, ptr %187, align 1
  %188 = add nuw i64 %180, 1
  %189 = load i64, ptr %8, align 8
  %190 = icmp sgt i64 %189, %188
  br i1 %190, label %rl_m_append__String_int8_t.exit78, label %191

191:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68
  %192 = shl i64 %188, 1
  %193 = tail call ptr @malloc(i64 %192)
  %194 = ptrtoint ptr %193 to i64
  %195 = icmp sgt i64 %192, 0
  br i1 %195, label %.lr.ph.preheader.i.i.i75, label %.lr.ph5.i.i.i73.preheader

.lr.ph.preheader.i.i.i75:                         ; preds = %191
  tail call void @llvm.memset.p0.i64(ptr align 1 %193, i8 0, i64 %192, i1 false)
  br label %.lr.ph5.i.i.i73.preheader

.lr.ph5.i.i.i73.preheader:                        ; preds = %191, %.lr.ph.preheader.i.i.i75
  %smax169 = tail call i64 @llvm.smax.i64(i64 %180, i64 1)
  %min.iters.check172 = icmp slt i64 %180, 4
  %196 = sub i64 %194, %185
  %diff.check168 = icmp ult i64 %196, 4
  %or.cond247 = or i1 %min.iters.check172, %diff.check168
  br i1 %or.cond247, label %.lr.ph5.i.i.i73.preheader250, label %vector.ph173

.lr.ph5.i.i.i73.preheader250:                     ; preds = %middle.block170, %.lr.ph5.i.i.i73.preheader
  %.14.i.i.i74.ph = phi i64 [ 0, %.lr.ph5.i.i.i73.preheader ], [ %n.vec175, %middle.block170 ]
  br label %.lr.ph5.i.i.i73

vector.ph173:                                     ; preds = %.lr.ph5.i.i.i73.preheader
  %n.vec175 = and i64 %smax169, 9223372036854775804
  br label %vector.body177

vector.body177:                                   ; preds = %vector.body177, %vector.ph173
  %index178 = phi i64 [ 0, %vector.ph173 ], [ %index.next180, %vector.body177 ]
  %197 = getelementptr i8, ptr %193, i64 %index178
  %198 = getelementptr i8, ptr %184, i64 %index178
  %wide.load179 = load <4 x i8>, ptr %198, align 1
  store <4 x i8> %wide.load179, ptr %197, align 1
  %index.next180 = add nuw i64 %index178, 4
  %199 = icmp eq i64 %index.next180, %n.vec175
  br i1 %199, label %middle.block170, label %vector.body177, !llvm.loop !17

middle.block170:                                  ; preds = %vector.body177
  %cmp.n181 = icmp eq i64 %smax169, %n.vec175
  br i1 %cmp.n181, label %.preheader.i.i.i71, label %.lr.ph5.i.i.i73.preheader250

.preheader.i.i.i71:                               ; preds = %.lr.ph5.i.i.i73, %middle.block170
  tail call void @free(ptr nonnull %184)
  store i64 %192, ptr %8, align 8
  store ptr %193, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit78

.lr.ph5.i.i.i73:                                  ; preds = %.lr.ph5.i.i.i73.preheader250, %.lr.ph5.i.i.i73
  %.14.i.i.i74 = phi i64 [ %203, %.lr.ph5.i.i.i73 ], [ %.14.i.i.i74.ph, %.lr.ph5.i.i.i73.preheader250 ]
  %200 = getelementptr i8, ptr %193, i64 %.14.i.i.i74
  %201 = getelementptr i8, ptr %184, i64 %.14.i.i.i74
  %202 = load i8, ptr %201, align 1
  store i8 %202, ptr %200, align 1
  %203 = add nuw nsw i64 %.14.i.i.i74, 1
  %204 = icmp slt i64 %203, %180
  br i1 %204, label %.lr.ph5.i.i.i73, label %.preheader.i.i.i71, !llvm.loop !18

rl_m_append__String_int8_t.exit78:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68, %.preheader.i.i.i71
  %205 = phi i64 [ %192, %.preheader.i.i.i71 ], [ %189, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68 ]
  %206 = phi ptr [ %193, %.preheader.i.i.i71 ], [ %184, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68 ]
  %207 = ptrtoint ptr %206 to i64
  %208 = getelementptr i8, ptr %206, i64 %180
  store i8 0, ptr %208, align 1
  %.not151 = icmp eq i64 %180, 9223372036854775807
  br i1 %.not151, label %209, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79

209:                                              ; preds = %rl_m_append__String_int8_t.exit78
  %210 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79:   ; preds = %rl_m_append__String_int8_t.exit78
  %211 = getelementptr i8, ptr %206, i64 %188
  %212 = getelementptr i8, ptr %211, i64 -1
  store i8 10, ptr %212, align 1
  %213 = add nuw i64 %180, 2
  %214 = icmp sgt i64 %205, %213
  br i1 %214, label %rl_m_append__String_int8_t.exit89, label %215

215:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79
  %216 = shl i64 %213, 1
  %217 = tail call ptr @malloc(i64 %216)
  %218 = ptrtoint ptr %217 to i64
  %219 = icmp sgt i64 %216, 0
  br i1 %219, label %.lr.ph.preheader.i.i.i86, label %.preheader2.i.i.i80

.lr.ph.preheader.i.i.i86:                         ; preds = %215
  tail call void @llvm.memset.p0.i64(ptr align 1 %217, i8 0, i64 %216, i1 false)
  br label %.preheader2.i.i.i80

.preheader2.i.i.i80:                              ; preds = %.lr.ph.preheader.i.i.i86, %215
  %smax154 = tail call i64 @llvm.smax.i64(i64 %188, i64 1)
  %min.iters.check157 = icmp slt i64 %188, 4
  %220 = sub i64 %218, %207
  %diff.check153 = icmp ult i64 %220, 4
  %or.cond248 = or i1 %min.iters.check157, %diff.check153
  br i1 %or.cond248, label %.lr.ph5.i.i.i84.preheader, label %vector.ph158

.lr.ph5.i.i.i84.preheader:                        ; preds = %middle.block155, %.preheader2.i.i.i80
  %.14.i.i.i85.ph = phi i64 [ 0, %.preheader2.i.i.i80 ], [ %n.vec160, %middle.block155 ]
  br label %.lr.ph5.i.i.i84

vector.ph158:                                     ; preds = %.preheader2.i.i.i80
  %n.vec160 = and i64 %smax154, 9223372036854775804
  br label %vector.body162

vector.body162:                                   ; preds = %vector.body162, %vector.ph158
  %index163 = phi i64 [ 0, %vector.ph158 ], [ %index.next165, %vector.body162 ]
  %221 = getelementptr i8, ptr %217, i64 %index163
  %222 = getelementptr i8, ptr %206, i64 %index163
  %wide.load164 = load <4 x i8>, ptr %222, align 1
  store <4 x i8> %wide.load164, ptr %221, align 1
  %index.next165 = add nuw i64 %index163, 4
  %223 = icmp eq i64 %index.next165, %n.vec160
  br i1 %223, label %middle.block155, label %vector.body162, !llvm.loop !19

middle.block155:                                  ; preds = %vector.body162
  %cmp.n166 = icmp eq i64 %smax154, %n.vec160
  br i1 %cmp.n166, label %.preheader.i.i.i82, label %.lr.ph5.i.i.i84.preheader

.preheader.i.i.i82:                               ; preds = %.lr.ph5.i.i.i84, %middle.block155
  tail call void @free(ptr nonnull %206)
  store i64 %216, ptr %8, align 8
  store ptr %217, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit89

.lr.ph5.i.i.i84:                                  ; preds = %.lr.ph5.i.i.i84.preheader, %.lr.ph5.i.i.i84
  %.14.i.i.i85 = phi i64 [ %227, %.lr.ph5.i.i.i84 ], [ %.14.i.i.i85.ph, %.lr.ph5.i.i.i84.preheader ]
  %224 = getelementptr i8, ptr %217, i64 %.14.i.i.i85
  %225 = getelementptr i8, ptr %206, i64 %.14.i.i.i85
  %226 = load i8, ptr %225, align 1
  store i8 %226, ptr %224, align 1
  %227 = add nuw nsw i64 %.14.i.i.i85, 1
  %228 = icmp slt i64 %227, %188
  br i1 %228, label %.lr.ph5.i.i.i84, label %.preheader.i.i.i82, !llvm.loop !20

rl_m_append__String_int8_t.exit89:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79, %.preheader.i.i.i82
  %229 = phi ptr [ %217, %.preheader.i.i.i82 ], [ %206, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79 ]
  %230 = getelementptr i8, ptr %229, i64 %188
  store i8 0, ptr %230, align 1
  store i64 %213, ptr %7, align 8
  %231 = add i64 %.0140148, 1
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %2)
  %.not1.i90 = icmp eq i64 %231, 0
  br i1 %.not1.i90, label %rl__indent_string__String_int64_t.exit94, label %.lr.ph.i91

.lr.ph.i91:                                       ; preds = %rl_m_append__String_int8_t.exit89, %.lr.ph.i91
  %.02.i92 = phi i64 [ %232, %.lr.ph.i91 ], [ 0, %rl_m_append__String_int8_t.exit89 ]
  store ptr @str_11, ptr %2, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %6, ptr nonnull %2)
  %232 = add nuw i64 %.02.i92, 1
  %.not.i93 = icmp eq i64 %.02.i92, %.0140148
  br i1 %.not.i93, label %rl__indent_string__String_int64_t.exit94, label %.lr.ph.i91

rl__indent_string__String_int64_t.exit94:         ; preds = %.lr.ph.i91, %rl_m_append__String_int8_t.exit89
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %2)
  br label %233

233:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit38, %rl_m_append__String_int8_t.exit66, %rl_m_append__String_int8_t.exit, %rl__indent_string__String_int64_t.exit94
  %.1141 = phi i64 [ %.0140148, %rl_m_append__String_int8_t.exit ], [ %144, %rl_m_append__String_int8_t.exit66 ], [ %231, %rl__indent_string__String_int64_t.exit94 ], [ %.0140148, %rl_m_get__String_int64_t_r_int8_tRef.exit38 ]
  %.1 = phi i64 [ %.0149, %rl_m_append__String_int8_t.exit ], [ %.0149, %rl_m_append__String_int8_t.exit66 ], [ %.0149, %rl__indent_string__String_int64_t.exit94 ], [ %spec.select, %rl_m_get__String_int64_t_r_int8_tRef.exit38 ]
  %234 = add i64 %.1, 1
  %235 = load i64, ptr %10, align 8
  %236 = add i64 %235, -1
  %237 = icmp slt i64 %234, %236
  br i1 %237, label %.lr.ph, label %._crit_edge.loopexit

._crit_edge.loopexit:                             ; preds = %233
  %.pre = load i64, ptr %8, align 8
  %238 = icmp eq i64 %.pre, 0
  %239 = getelementptr inbounds i8, ptr %5, i64 8
  %240 = getelementptr inbounds i8, ptr %5, i64 16
  store i64 4, ptr %240, align 8
  %241 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %241, ptr %5, align 8
  store i32 0, ptr %241, align 1
  store i64 1, ptr %239, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %5, ptr nonnull readonly %6)
  br i1 %238, label %rl_m_drop__String.exit, label %245

.critedge:                                        ; preds = %rl_m_init__String.exit
  %242 = getelementptr inbounds i8, ptr %5, i64 8
  %243 = getelementptr inbounds i8, ptr %5, i64 16
  store i64 4, ptr %243, align 8
  %244 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %244, ptr %5, align 8
  store i32 0, ptr %244, align 1
  store i64 1, ptr %242, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %5, ptr nonnull readonly %6)
  br label %245

245:                                              ; preds = %.critedge, %._crit_edge.loopexit
  %246 = load ptr, ptr %6, align 8
  tail call void @free(ptr %246)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %._crit_edge.loopexit, %245
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 1 dereferenceable(24) %0, ptr noundef nonnull align 8 dereferenceable(24) %5, i64 24, i1 false)
  ret void
}

; Function Attrs: nounwind
define void @rl_m_reverse__String(ptr nocapture readonly %0) local_unnamed_addr #5 {
  %2 = getelementptr i8, ptr %0, i64 8
  %3 = load i64, ptr %2, align 8
  %storemerge14 = add i64 %3, -2
  %4 = icmp sgt i64 %storemerge14, 0
  br i1 %4, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %1, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit3
  %storemerge16 = phi i64 [ %storemerge, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit3 ], [ %storemerge14, %1 ]
  %.015 = phi i64 [ %25, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit3 ], [ 0, %1 ]
  %5 = load i64, ptr %2, align 8
  %6 = icmp slt i64 %.015, %5
  br i1 %6, label %9, label %7

7:                                                ; preds = %.lr.ph
  %8 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

9:                                                ; preds = %.lr.ph
  %10 = load ptr, ptr %0, align 8
  %11 = getelementptr i8, ptr %10, i64 %.015
  %12 = load i8, ptr %11, align 1
  %13 = icmp ult i64 %storemerge16, %5
  br i1 %13, label %16, label %14

14:                                               ; preds = %9
  %15 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

16:                                               ; preds = %9
  %17 = getelementptr i8, ptr %10, i64 %storemerge16
  %18 = load i8, ptr %17, align 1
  store i8 %18, ptr %11, align 1
  %19 = load i64, ptr %2, align 8
  %20 = icmp slt i64 %storemerge16, %19
  br i1 %20, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit3, label %21

21:                                               ; preds = %16
  %22 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit3: ; preds = %16
  %23 = load ptr, ptr %0, align 8
  %24 = getelementptr i8, ptr %23, i64 %storemerge16
  store i8 %12, ptr %24, align 1
  %25 = add nuw nsw i64 %.015, 1
  %storemerge = add nsw i64 %storemerge16, -1
  %26 = icmp slt i64 %25, %storemerge
  br i1 %26, label %.lr.ph, label %._crit_edge

._crit_edge:                                      ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit3, %1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_back__String_r_int8_tRef(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %1, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = add i64 %4, -2
  %6 = icmp sgt i64 %5, -1
  br i1 %6, label %9, label %7

7:                                                ; preds = %2
  %8 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

9:                                                ; preds = %2
  %10 = icmp sgt i64 %4, -9223372036854775807
  br i1 %10, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit, label %11

11:                                               ; preds = %9
  %12 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit: ; preds = %9
  %13 = load ptr, ptr %1, align 8
  %14 = getelementptr i8, ptr %13, i64 %5
  store ptr %14, ptr %0, align 8
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(write, argmem: readwrite, inaccessiblemem: none)
define void @rl_m_drop_back__String_int64_t(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #8 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = load i64, ptr %1, align 8
  %6 = sub i64 %4, %5
  %7 = icmp slt i64 %6, %4
  br i1 %7, label %.lr.ph.i, label %rl_m_drop_back__VectorTint8_tT_int64_t.exit

.lr.ph.i:                                         ; preds = %2, %.lr.ph.i
  %.02.i = phi i64 [ %10, %.lr.ph.i ], [ %6, %2 ]
  %8 = load ptr, ptr %0, align 8
  %9 = getelementptr i8, ptr %8, i64 %.02.i
  store i8 0, ptr %9, align 1
  %10 = add nsw i64 %.02.i, 1
  %11 = load i64, ptr %3, align 8
  %12 = icmp slt i64 %10, %11
  br i1 %12, label %.lr.ph.i, label %._crit_edge.loopexit.i

._crit_edge.loopexit.i:                           ; preds = %.lr.ph.i
  %.pre.i = load i64, ptr %1, align 8
  %.pre4.i = sub i64 %11, %.pre.i
  br label %rl_m_drop_back__VectorTint8_tT_int64_t.exit

rl_m_drop_back__VectorTint8_tT_int64_t.exit:      ; preds = %2, %._crit_edge.loopexit.i
  %.pre-phi.i = phi i64 [ %.pre4.i, %._crit_edge.loopexit.i ], [ %6, %2 ]
  store i64 %.pre-phi.i, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_not_equal__String_strlit_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = getelementptr i8, ptr %1, i64 8
  %5 = load i64, ptr %4, align 8
  %6 = add i64 %5, -1
  %7 = icmp sgt i64 %6, 0
  br i1 %7, label %.lr.ph.i, label %.._crit_edge_crit_edge.i

.._crit_edge_crit_edge.i:                         ; preds = %3
  %.pre.i = load ptr, ptr %2, align 8
  br label %._crit_edge.i

.lr.ph.i:                                         ; preds = %3, %18
  %storemerge12.i = phi i64 [ %19, %18 ], [ 0, %3 ]
  %8 = icmp slt i64 %storemerge12.i, %5
  br i1 %8, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i, label %9

9:                                                ; preds = %.lr.ph.i
  %10 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit.i:      ; preds = %.lr.ph.i
  %11 = load ptr, ptr %1, align 8
  %12 = getelementptr i8, ptr %11, i64 %storemerge12.i
  %13 = load ptr, ptr %2, align 8
  %14 = getelementptr i8, ptr %13, i64 %storemerge12.i
  %15 = load i8, ptr %12, align 1
  %16 = load i8, ptr %14, align 1
  %.not5.i = icmp ne i8 %15, %16
  %17 = icmp eq i8 %15, 0
  %or.cond.i = or i1 %17, %.not5.i
  br i1 %or.cond.i, label %rl_m_equal__String_strlit_r_bool.exit, label %18

18:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i
  %19 = add nuw nsw i64 %storemerge12.i, 1
  %20 = icmp slt i64 %19, %6
  br i1 %20, label %.lr.ph.i, label %._crit_edge.i

._crit_edge.i:                                    ; preds = %18, %.._crit_edge_crit_edge.i
  %21 = phi ptr [ %.pre.i, %.._crit_edge_crit_edge.i ], [ %13, %18 ]
  %storemerge.lcssa.i = phi i64 [ 0, %.._crit_edge_crit_edge.i ], [ %6, %18 ]
  %22 = getelementptr i8, ptr %21, i64 %storemerge.lcssa.i
  %23 = load i8, ptr %22, align 1
  %.not.i = icmp ne i8 %23, 0
  br label %rl_m_equal__String_strlit_r_bool.exit

rl_m_equal__String_strlit_r_bool.exit:            ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i, %._crit_edge.i
  %.sink.i = phi i1 [ %.not.i, %._crit_edge.i ], [ true, %rl_m_get__String_int64_t_r_int8_tRef.exit.i ]
  %24 = zext i1 %.sink.i to i8
  store i8 %24, ptr %0, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_not_equal__String_String_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = getelementptr i8, ptr %2, i64 8
  %5 = load i64, ptr %4, align 8
  %6 = getelementptr i8, ptr %1, i64 8
  %7 = load i64, ptr %6, align 8
  %8 = add i64 %7, -1
  %.not.i = icmp eq i64 %5, %7
  br i1 %.not.i, label %.preheader.i, label %rl_m_equal__String_String_r_bool.exit

.preheader.i:                                     ; preds = %3
  %9 = icmp sgt i64 %8, 0
  br i1 %9, label %.lr.ph.i.peel, label %rl_m_equal__String_String_r_bool.exit

.lr.ph.i.peel:                                    ; preds = %.preheader.i
  %10 = icmp sgt i64 %5, 0
  br i1 %10, label %rl_m_get__String_int64_t_r_int8_tRef.exit4.i.peel, label %.loopexit

rl_m_get__String_int64_t_r_int8_tRef.exit4.i.peel: ; preds = %.lr.ph.i.peel
  %11 = load ptr, ptr %1, align 8
  %12 = load ptr, ptr %2, align 8
  %13 = load i8, ptr %11, align 1
  %14 = load i8, ptr %12, align 1
  %.not3.i.not.peel = icmp ne i8 %13, %14
  %15 = icmp eq i64 %8, 1
  %or.cond.not.peel = or i1 %.not3.i.not.peel, %15
  br i1 %or.cond.not.peel, label %rl_m_equal__String_String_r_bool.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit4.i.peel, %rl_m_get__String_int64_t_r_int8_tRef.exit4.i
  %storemerge12.i = phi i64 [ %24, %rl_m_get__String_int64_t_r_int8_tRef.exit4.i ], [ 1, %rl_m_get__String_int64_t_r_int8_tRef.exit4.i.peel ]
  %16 = icmp slt i64 %storemerge12.i, %5
  br i1 %16, label %rl_m_get__String_int64_t_r_int8_tRef.exit4.i, label %.loopexit

.loopexit:                                        ; preds = %.lr.ph.i, %.lr.ph.i.peel
  %17 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit4.i:     ; preds = %.lr.ph.i
  %18 = load ptr, ptr %1, align 8
  %19 = getelementptr i8, ptr %18, i64 %storemerge12.i
  %20 = load ptr, ptr %2, align 8
  %21 = getelementptr i8, ptr %20, i64 %storemerge12.i
  %22 = load i8, ptr %19, align 1
  %23 = load i8, ptr %21, align 1
  %.not3.i.not = icmp ne i8 %22, %23
  %24 = add nuw nsw i64 %storemerge12.i, 1
  %25 = icmp sge i64 %24, %8
  %or.cond.not = select i1 %.not3.i.not, i1 true, i1 %25
  br i1 %or.cond.not, label %rl_m_equal__String_String_r_bool.exit, label %.lr.ph.i, !llvm.loop !21

rl_m_equal__String_String_r_bool.exit:            ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit4.i.peel, %rl_m_get__String_int64_t_r_int8_tRef.exit4.i, %3, %.preheader.i
  %26 = phi i1 [ false, %.preheader.i ], [ true, %3 ], [ %.not3.i.not.peel, %rl_m_get__String_int64_t_r_int8_tRef.exit4.i.peel ], [ %.not3.i.not, %rl_m_get__String_int64_t_r_int8_tRef.exit4.i ]
  %27 = zext i1 %26 to i8
  store i8 %27, ptr %0, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_equal__String_String_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = getelementptr i8, ptr %2, i64 8
  %5 = load i64, ptr %4, align 8
  %6 = getelementptr i8, ptr %1, i64 8
  %7 = load i64, ptr %6, align 8
  %8 = add i64 %7, -1
  %.not = icmp eq i64 %5, %7
  br i1 %.not, label %.preheader, label %common.ret

.preheader:                                       ; preds = %3
  %9 = icmp sgt i64 %8, 0
  br i1 %9, label %.lr.ph, label %common.ret

10:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit4
  %11 = add nuw nsw i64 %storemerge12, 1
  %12 = icmp slt i64 %11, %8
  br i1 %12, label %.lr.ph, label %common.ret

.lr.ph:                                           ; preds = %.preheader, %10
  %storemerge12 = phi i64 [ %11, %10 ], [ 0, %.preheader ]
  %13 = icmp slt i64 %storemerge12, %5
  br i1 %13, label %rl_m_get__String_int64_t_r_int8_tRef.exit4, label %14

14:                                               ; preds = %.lr.ph
  %15 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit4:       ; preds = %.lr.ph
  %16 = load ptr, ptr %1, align 8
  %17 = getelementptr i8, ptr %16, i64 %storemerge12
  %18 = load ptr, ptr %2, align 8
  %19 = getelementptr i8, ptr %18, i64 %storemerge12
  %20 = load i8, ptr %17, align 1
  %21 = load i8, ptr %19, align 1
  %.not3 = icmp eq i8 %20, %21
  br i1 %.not3, label %10, label %common.ret

common.ret:                                       ; preds = %10, %rl_m_get__String_int64_t_r_int8_tRef.exit4, %3, %.preheader
  %.sink = phi i8 [ 1, %.preheader ], [ 0, %3 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit4 ], [ 1, %10 ]
  store i8 %.sink, ptr %0, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_equal__String_strlit_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = getelementptr i8, ptr %1, i64 8
  %5 = load i64, ptr %4, align 8
  %6 = add i64 %5, -1
  %7 = icmp sgt i64 %6, 0
  br i1 %7, label %.lr.ph, label %.._crit_edge_crit_edge

.._crit_edge_crit_edge:                           ; preds = %3
  %.pre = load ptr, ptr %2, align 8
  br label %._crit_edge

.lr.ph:                                           ; preds = %3, %18
  %storemerge12 = phi i64 [ %19, %18 ], [ 0, %3 ]
  %8 = icmp slt i64 %storemerge12, %5
  br i1 %8, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %9

9:                                                ; preds = %.lr.ph
  %10 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %.lr.ph
  %11 = load ptr, ptr %1, align 8
  %12 = getelementptr i8, ptr %11, i64 %storemerge12
  %13 = load ptr, ptr %2, align 8
  %14 = getelementptr i8, ptr %13, i64 %storemerge12
  %15 = load i8, ptr %12, align 1
  %16 = load i8, ptr %14, align 1
  %.not5 = icmp ne i8 %15, %16
  %17 = icmp eq i8 %15, 0
  %or.cond = or i1 %.not5, %17
  br i1 %or.cond, label %common.ret, label %18

18:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %19 = add nuw nsw i64 %storemerge12, 1
  %20 = icmp slt i64 %19, %6
  br i1 %20, label %.lr.ph, label %._crit_edge

common.ret:                                       ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit, %._crit_edge
  %.sink = phi i8 [ %., %._crit_edge ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit ]
  store i8 %.sink, ptr %0, align 1
  ret void

._crit_edge:                                      ; preds = %18, %.._crit_edge_crit_edge
  %21 = phi ptr [ %.pre, %.._crit_edge_crit_edge ], [ %13, %18 ]
  %storemerge.lcssa = phi i64 [ 0, %.._crit_edge_crit_edge ], [ %6, %18 ]
  %22 = getelementptr i8, ptr %21, i64 %storemerge.lcssa
  %23 = load i8, ptr %22, align 1
  %.not = icmp eq i8 %23, 0
  %. = zext i1 %.not to i8
  br label %common.ret
}

; Function Attrs: nounwind
define void @rl_m_add__String_String_r_String(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
rl_m_init__String.exit:
  %3 = alloca %String, align 8
  %4 = alloca %String, align 8
  %5 = getelementptr inbounds i8, ptr %4, i64 8
  %6 = getelementptr inbounds i8, ptr %4, i64 16
  store i64 4, ptr %6, align 8
  %7 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %7, ptr %4, align 8
  store i32 0, ptr %7, align 1
  store i64 1, ptr %5, align 8
  call void @rl_m_append__String_String(ptr nonnull %4, ptr %1)
  call void @rl_m_append__String_String(ptr nonnull %4, ptr %2)
  %8 = getelementptr inbounds i8, ptr %3, i64 8
  %9 = getelementptr inbounds i8, ptr %3, i64 16
  store i64 4, ptr %9, align 8
  %10 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %10, ptr %3, align 8
  store i32 0, ptr %10, align 1
  store i64 1, ptr %8, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %3, ptr nonnull readonly %4)
  %11 = load i64, ptr %6, align 8
  %.not2.i.i = icmp eq i64 %11, 0
  br i1 %.not2.i.i, label %rl_m_drop__String.exit, label %12

12:                                               ; preds = %rl_m_init__String.exit
  %13 = load ptr, ptr %4, align 8
  tail call void @free(ptr %13)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %rl_m_init__String.exit, %12
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 1 dereferenceable(24) %0, ptr noundef nonnull align 8 dereferenceable(24) %3, i64 24, i1 false)
  ret void
}

; Function Attrs: nounwind
define void @rl_m_append_quoted__String_String(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %4, 0
  br i1 %5, label %rl_m_pop__VectorTint8_tT_r_int8_t.exit, label %6

6:                                                ; preds = %2
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_7)
  tail call void @llvm.trap()
  unreachable

rl_m_pop__VectorTint8_tT_r_int8_t.exit:           ; preds = %2
  %8 = add nsw i64 %4, -1
  %9 = load ptr, ptr %0, align 8
  %10 = getelementptr i8, ptr %9, i64 %8
  store i64 %8, ptr %3, align 8
  store i8 0, ptr %10, align 1
  %11 = load i64, ptr %3, align 8
  %12 = add i64 %11, 1
  %13 = getelementptr i8, ptr %0, i64 16
  %14 = load i64, ptr %13, align 8
  %15 = icmp sgt i64 %14, %12
  br i1 %15, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, label %16

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i: ; preds = %rl_m_pop__VectorTint8_tT_r_int8_t.exit
  %.pre2.i = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

16:                                               ; preds = %rl_m_pop__VectorTint8_tT_r_int8_t.exit
  %17 = shl i64 %12, 1
  %18 = tail call ptr @malloc(i64 %17)
  %19 = ptrtoint ptr %18 to i64
  %20 = icmp sgt i64 %17, 0
  br i1 %20, label %.lr.ph.preheader.i.i, label %.preheader2.i.i

.lr.ph.preheader.i.i:                             ; preds = %16
  tail call void @llvm.memset.p0.i64(ptr align 1 %18, i8 0, i64 %17, i1 false)
  br label %.preheader2.i.i

.preheader2.i.i:                                  ; preds = %.lr.ph.preheader.i.i, %16
  %21 = icmp sgt i64 %11, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %21, label %.lr.ph5.i.i.preheader, label %.preheader.i.i

.lr.ph5.i.i.preheader:                            ; preds = %.preheader2.i.i
  %.pre.i.i52 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %11, 4
  %22 = sub i64 %19, %.pre.i.i52
  %diff.check = icmp ult i64 %22, 4
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph5.i.i.preheader121, label %vector.ph

.lr.ph5.i.i.preheader121:                         ; preds = %middle.block, %.lr.ph5.i.i.preheader
  %.14.i.i.ph = phi i64 [ 0, %.lr.ph5.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph5.i.i

vector.ph:                                        ; preds = %.lr.ph5.i.i.preheader
  %n.vec = and i64 %11, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %23 = getelementptr i8, ptr %18, i64 %index
  %24 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %wide.load = load <4 x i8>, ptr %24, align 1
  store <4 x i8> %wide.load, ptr %23, align 1
  %index.next = add nuw i64 %index, 4
  %25 = icmp eq i64 %index.next, %n.vec
  br i1 %25, label %middle.block, label %vector.body, !llvm.loop !23

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %11, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph5.i.i.preheader121

.preheader.i.i:                                   ; preds = %.lr.ph5.i.i, %middle.block, %.preheader2.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %17, ptr %13, align 8
  store ptr %18, ptr %0, align 8
  %.pre.i = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph5.i.i:                                      ; preds = %.lr.ph5.i.i.preheader121, %.lr.ph5.i.i
  %.14.i.i = phi i64 [ %29, %.lr.ph5.i.i ], [ %.14.i.i.ph, %.lr.ph5.i.i.preheader121 ]
  %26 = getelementptr i8, ptr %18, i64 %.14.i.i
  %27 = getelementptr i8, ptr %.pre.i.i, i64 %.14.i.i
  %28 = load i8, ptr %27, align 1
  store i8 %28, ptr %26, align 1
  %29 = add nuw nsw i64 %.14.i.i, 1
  %30 = icmp slt i64 %29, %11
  br i1 %30, label %.lr.ph5.i.i, label %.preheader.i.i, !llvm.loop !24

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %31 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %18, %.preheader.i.i ]
  %32 = phi i64 [ %11, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %33 = getelementptr i8, ptr %31, i64 %32
  store i8 34, ptr %33, align 1
  %34 = load i64, ptr %3, align 8
  %35 = add i64 %34, 1
  store i64 %35, ptr %3, align 8
  %36 = getelementptr i8, ptr %1, i64 8
  %37 = load i64, ptr %36, align 8
  %38 = add i64 %37, -1
  %39 = icmp sgt i64 %38, 0
  br i1 %39, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit, %rl_m_append__VectorTint8_tT_int8_t.exit21
  %40 = phi i64 [ %104, %rl_m_append__VectorTint8_tT_int8_t.exit21 ], [ %35, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %41 = phi i64 [ %106, %rl_m_append__VectorTint8_tT_int8_t.exit21 ], [ %37, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %storemerge51 = phi i64 [ %105, %rl_m_append__VectorTint8_tT_int8_t.exit21 ], [ 0, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %42 = icmp slt i64 %storemerge51, %41
  br i1 %42, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %43

43:                                               ; preds = %.lr.ph.preheader
  %44 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %.lr.ph.preheader
  %45 = load ptr, ptr %1, align 8
  %46 = getelementptr i8, ptr %45, i64 %storemerge51
  %47 = load i8, ptr %46, align 1
  %48 = icmp eq i8 %47, 34
  br i1 %48, label %49, label %73

49:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %50 = add i64 %40, 1
  %51 = load i64, ptr %13, align 8
  %52 = icmp sgt i64 %51, %50
  br i1 %52, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8, label %53

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8: ; preds = %49
  %.pre2.i9 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit10

53:                                               ; preds = %49
  %54 = shl i64 %50, 1
  %55 = tail call ptr @malloc(i64 %54)
  %56 = ptrtoint ptr %55 to i64
  %57 = icmp sgt i64 %54, 0
  br i1 %57, label %.lr.ph.preheader.i.i7, label %.preheader2.i.i1

.lr.ph.preheader.i.i7:                            ; preds = %53
  tail call void @llvm.memset.p0.i64(ptr align 1 %55, i8 0, i64 %54, i1 false)
  br label %.preheader2.i.i1

.preheader2.i.i1:                                 ; preds = %.lr.ph.preheader.i.i7, %53
  %58 = icmp sgt i64 %40, 0
  %.pre.i.i2 = load ptr, ptr %0, align 8
  br i1 %58, label %.lr.ph5.i.i5.preheader, label %.preheader.i.i3

.lr.ph5.i.i5.preheader:                           ; preds = %.preheader2.i.i1
  %.pre.i.i269 = ptrtoint ptr %.pre.i.i2 to i64
  %min.iters.check73 = icmp ult i64 %40, 4
  %59 = sub i64 %56, %.pre.i.i269
  %diff.check70 = icmp ult i64 %59, 4
  %or.cond113 = select i1 %min.iters.check73, i1 true, i1 %diff.check70
  br i1 %or.cond113, label %.lr.ph5.i.i5.preheader120, label %vector.ph74

.lr.ph5.i.i5.preheader120:                        ; preds = %middle.block71, %.lr.ph5.i.i5.preheader
  %.14.i.i6.ph = phi i64 [ 0, %.lr.ph5.i.i5.preheader ], [ %n.vec76, %middle.block71 ]
  br label %.lr.ph5.i.i5

vector.ph74:                                      ; preds = %.lr.ph5.i.i5.preheader
  %n.vec76 = and i64 %40, 9223372036854775804
  br label %vector.body78

vector.body78:                                    ; preds = %vector.body78, %vector.ph74
  %index79 = phi i64 [ 0, %vector.ph74 ], [ %index.next81, %vector.body78 ]
  %60 = getelementptr i8, ptr %55, i64 %index79
  %61 = getelementptr i8, ptr %.pre.i.i2, i64 %index79
  %wide.load80 = load <4 x i8>, ptr %61, align 1
  store <4 x i8> %wide.load80, ptr %60, align 1
  %index.next81 = add nuw i64 %index79, 4
  %62 = icmp eq i64 %index.next81, %n.vec76
  br i1 %62, label %middle.block71, label %vector.body78, !llvm.loop !25

middle.block71:                                   ; preds = %vector.body78
  %cmp.n82 = icmp eq i64 %40, %n.vec76
  br i1 %cmp.n82, label %.preheader.i.i3, label %.lr.ph5.i.i5.preheader120

.preheader.i.i3:                                  ; preds = %.lr.ph5.i.i5, %middle.block71, %.preheader2.i.i1
  tail call void @free(ptr %.pre.i.i2)
  store i64 %54, ptr %13, align 8
  store ptr %55, ptr %0, align 8
  %.pre.i4 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit10

.lr.ph5.i.i5:                                     ; preds = %.lr.ph5.i.i5.preheader120, %.lr.ph5.i.i5
  %.14.i.i6 = phi i64 [ %66, %.lr.ph5.i.i5 ], [ %.14.i.i6.ph, %.lr.ph5.i.i5.preheader120 ]
  %63 = getelementptr i8, ptr %55, i64 %.14.i.i6
  %64 = getelementptr i8, ptr %.pre.i.i2, i64 %.14.i.i6
  %65 = load i8, ptr %64, align 1
  store i8 %65, ptr %63, align 1
  %66 = add nuw nsw i64 %.14.i.i6, 1
  %67 = icmp slt i64 %66, %40
  br i1 %67, label %.lr.ph5.i.i5, label %.preheader.i.i3, !llvm.loop !26

rl_m_append__VectorTint8_tT_int8_t.exit10:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8, %.preheader.i.i3
  %68 = phi ptr [ %.pre2.i9, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8 ], [ %55, %.preheader.i.i3 ]
  %69 = phi i64 [ %40, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8 ], [ %.pre.i4, %.preheader.i.i3 ]
  %70 = getelementptr i8, ptr %68, i64 %69
  store i8 92, ptr %70, align 1
  %71 = load i64, ptr %3, align 8
  %72 = add i64 %71, 1
  store i64 %72, ptr %3, align 8
  %.pre = load i64, ptr %36, align 8
  br label %73

73:                                               ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit10, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %74 = phi i64 [ %72, %rl_m_append__VectorTint8_tT_int8_t.exit10 ], [ %40, %rl_m_get__String_int64_t_r_int8_tRef.exit ]
  %75 = phi i64 [ %.pre, %rl_m_append__VectorTint8_tT_int8_t.exit10 ], [ %41, %rl_m_get__String_int64_t_r_int8_tRef.exit ]
  %76 = icmp slt i64 %storemerge51, %75
  br i1 %76, label %rl_m_get__String_int64_t_r_int8_tRef.exit11, label %77

77:                                               ; preds = %73
  %78 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit11:      ; preds = %73
  %79 = load ptr, ptr %1, align 8
  %80 = getelementptr i8, ptr %79, i64 %storemerge51
  %81 = add i64 %74, 1
  %82 = load i64, ptr %13, align 8
  %83 = icmp sgt i64 %82, %81
  br i1 %83, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i19, label %84

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i19: ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit11
  %.pre2.i20 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit21

84:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit11
  %85 = shl i64 %81, 1
  %86 = tail call ptr @malloc(i64 %85)
  %87 = ptrtoint ptr %86 to i64
  %88 = icmp sgt i64 %85, 0
  br i1 %88, label %.lr.ph.preheader.i.i18, label %.preheader2.i.i12

.lr.ph.preheader.i.i18:                           ; preds = %84
  tail call void @llvm.memset.p0.i64(ptr align 1 %86, i8 0, i64 %85, i1 false)
  br label %.preheader2.i.i12

.preheader2.i.i12:                                ; preds = %.lr.ph.preheader.i.i18, %84
  %89 = icmp sgt i64 %74, 0
  %.pre.i.i13 = load ptr, ptr %0, align 8
  br i1 %89, label %.lr.ph5.i.i16.preheader, label %.preheader.i.i14

.lr.ph5.i.i16.preheader:                          ; preds = %.preheader2.i.i12
  %.pre.i.i1354 = ptrtoint ptr %.pre.i.i13 to i64
  %min.iters.check58 = icmp ult i64 %74, 4
  %90 = sub i64 %87, %.pre.i.i1354
  %diff.check55 = icmp ult i64 %90, 4
  %or.cond114 = select i1 %min.iters.check58, i1 true, i1 %diff.check55
  br i1 %or.cond114, label %.lr.ph5.i.i16.preheader119, label %vector.ph59

.lr.ph5.i.i16.preheader119:                       ; preds = %middle.block56, %.lr.ph5.i.i16.preheader
  %.14.i.i17.ph = phi i64 [ 0, %.lr.ph5.i.i16.preheader ], [ %n.vec61, %middle.block56 ]
  br label %.lr.ph5.i.i16

vector.ph59:                                      ; preds = %.lr.ph5.i.i16.preheader
  %n.vec61 = and i64 %74, 9223372036854775804
  br label %vector.body63

vector.body63:                                    ; preds = %vector.body63, %vector.ph59
  %index64 = phi i64 [ 0, %vector.ph59 ], [ %index.next66, %vector.body63 ]
  %91 = getelementptr i8, ptr %86, i64 %index64
  %92 = getelementptr i8, ptr %.pre.i.i13, i64 %index64
  %wide.load65 = load <4 x i8>, ptr %92, align 1
  store <4 x i8> %wide.load65, ptr %91, align 1
  %index.next66 = add nuw i64 %index64, 4
  %93 = icmp eq i64 %index.next66, %n.vec61
  br i1 %93, label %middle.block56, label %vector.body63, !llvm.loop !27

middle.block56:                                   ; preds = %vector.body63
  %cmp.n67 = icmp eq i64 %74, %n.vec61
  br i1 %cmp.n67, label %.preheader.i.i14, label %.lr.ph5.i.i16.preheader119

.preheader.i.i14:                                 ; preds = %.lr.ph5.i.i16, %middle.block56, %.preheader2.i.i12
  tail call void @free(ptr %.pre.i.i13)
  store i64 %85, ptr %13, align 8
  store ptr %86, ptr %0, align 8
  %.pre.i15 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit21

.lr.ph5.i.i16:                                    ; preds = %.lr.ph5.i.i16.preheader119, %.lr.ph5.i.i16
  %.14.i.i17 = phi i64 [ %97, %.lr.ph5.i.i16 ], [ %.14.i.i17.ph, %.lr.ph5.i.i16.preheader119 ]
  %94 = getelementptr i8, ptr %86, i64 %.14.i.i17
  %95 = getelementptr i8, ptr %.pre.i.i13, i64 %.14.i.i17
  %96 = load i8, ptr %95, align 1
  store i8 %96, ptr %94, align 1
  %97 = add nuw nsw i64 %.14.i.i17, 1
  %98 = icmp slt i64 %97, %74
  br i1 %98, label %.lr.ph5.i.i16, label %.preheader.i.i14, !llvm.loop !28

rl_m_append__VectorTint8_tT_int8_t.exit21:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i19, %.preheader.i.i14
  %99 = phi ptr [ %.pre2.i20, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i19 ], [ %86, %.preheader.i.i14 ]
  %100 = phi i64 [ %74, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i19 ], [ %.pre.i15, %.preheader.i.i14 ]
  %101 = getelementptr i8, ptr %99, i64 %100
  %102 = load i8, ptr %80, align 1
  store i8 %102, ptr %101, align 1
  %103 = load i64, ptr %3, align 8
  %104 = add i64 %103, 1
  store i64 %104, ptr %3, align 8
  %105 = add nuw nsw i64 %storemerge51, 1
  %106 = load i64, ptr %36, align 8
  %107 = add i64 %106, -1
  %108 = icmp slt i64 %105, %107
  br i1 %108, label %.lr.ph.preheader, label %._crit_edge

._crit_edge:                                      ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit21, %rl_m_append__VectorTint8_tT_int8_t.exit
  %109 = phi i64 [ %35, %rl_m_append__VectorTint8_tT_int8_t.exit ], [ %104, %rl_m_append__VectorTint8_tT_int8_t.exit21 ]
  %110 = add i64 %109, 1
  %111 = load i64, ptr %13, align 8
  %112 = icmp sgt i64 %111, %110
  br i1 %112, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i29, label %113

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i29: ; preds = %._crit_edge
  %.pre2.i30 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit31

113:                                              ; preds = %._crit_edge
  %114 = shl i64 %110, 1
  %115 = tail call ptr @malloc(i64 %114)
  %116 = ptrtoint ptr %115 to i64
  %117 = icmp sgt i64 %114, 0
  br i1 %117, label %.lr.ph.preheader.i.i28, label %.preheader2.i.i22

.lr.ph.preheader.i.i28:                           ; preds = %113
  tail call void @llvm.memset.p0.i64(ptr align 1 %115, i8 0, i64 %114, i1 false)
  br label %.preheader2.i.i22

.preheader2.i.i22:                                ; preds = %.lr.ph.preheader.i.i28, %113
  %118 = icmp sgt i64 %109, 0
  %.pre.i.i23 = load ptr, ptr %0, align 8
  br i1 %118, label %.lr.ph5.i.i26.preheader, label %.preheader.i.i24

.lr.ph5.i.i26.preheader:                          ; preds = %.preheader2.i.i22
  %.pre.i.i2384 = ptrtoint ptr %.pre.i.i23 to i64
  %min.iters.check88 = icmp ult i64 %109, 4
  %119 = sub i64 %116, %.pre.i.i2384
  %diff.check85 = icmp ult i64 %119, 4
  %or.cond115 = select i1 %min.iters.check88, i1 true, i1 %diff.check85
  br i1 %or.cond115, label %.lr.ph5.i.i26.preheader118, label %vector.ph89

.lr.ph5.i.i26.preheader118:                       ; preds = %middle.block86, %.lr.ph5.i.i26.preheader
  %.14.i.i27.ph = phi i64 [ 0, %.lr.ph5.i.i26.preheader ], [ %n.vec91, %middle.block86 ]
  br label %.lr.ph5.i.i26

vector.ph89:                                      ; preds = %.lr.ph5.i.i26.preheader
  %n.vec91 = and i64 %109, 9223372036854775804
  br label %vector.body93

vector.body93:                                    ; preds = %vector.body93, %vector.ph89
  %index94 = phi i64 [ 0, %vector.ph89 ], [ %index.next96, %vector.body93 ]
  %120 = getelementptr i8, ptr %115, i64 %index94
  %121 = getelementptr i8, ptr %.pre.i.i23, i64 %index94
  %wide.load95 = load <4 x i8>, ptr %121, align 1
  store <4 x i8> %wide.load95, ptr %120, align 1
  %index.next96 = add nuw i64 %index94, 4
  %122 = icmp eq i64 %index.next96, %n.vec91
  br i1 %122, label %middle.block86, label %vector.body93, !llvm.loop !29

middle.block86:                                   ; preds = %vector.body93
  %cmp.n97 = icmp eq i64 %109, %n.vec91
  br i1 %cmp.n97, label %.preheader.i.i24, label %.lr.ph5.i.i26.preheader118

.preheader.i.i24:                                 ; preds = %.lr.ph5.i.i26, %middle.block86, %.preheader2.i.i22
  tail call void @free(ptr %.pre.i.i23)
  store i64 %114, ptr %13, align 8
  store ptr %115, ptr %0, align 8
  %.pre.i25 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit31

.lr.ph5.i.i26:                                    ; preds = %.lr.ph5.i.i26.preheader118, %.lr.ph5.i.i26
  %.14.i.i27 = phi i64 [ %126, %.lr.ph5.i.i26 ], [ %.14.i.i27.ph, %.lr.ph5.i.i26.preheader118 ]
  %123 = getelementptr i8, ptr %115, i64 %.14.i.i27
  %124 = getelementptr i8, ptr %.pre.i.i23, i64 %.14.i.i27
  %125 = load i8, ptr %124, align 1
  store i8 %125, ptr %123, align 1
  %126 = add nuw nsw i64 %.14.i.i27, 1
  %127 = icmp slt i64 %126, %109
  br i1 %127, label %.lr.ph5.i.i26, label %.preheader.i.i24, !llvm.loop !30

rl_m_append__VectorTint8_tT_int8_t.exit31:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i29, %.preheader.i.i24
  %128 = phi ptr [ %.pre2.i30, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i29 ], [ %115, %.preheader.i.i24 ]
  %129 = phi i64 [ %109, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i29 ], [ %.pre.i25, %.preheader.i.i24 ]
  %130 = getelementptr i8, ptr %128, i64 %129
  store i8 34, ptr %130, align 1
  %131 = load i64, ptr %3, align 8
  %132 = add i64 %131, 1
  store i64 %132, ptr %3, align 8
  %133 = add i64 %131, 2
  %134 = load i64, ptr %13, align 8
  %135 = icmp sgt i64 %134, %133
  br i1 %135, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i39, label %136

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i39: ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit31
  %.pre2.i40 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit41

136:                                              ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit31
  %137 = shl i64 %133, 1
  %138 = tail call ptr @malloc(i64 %137)
  %139 = ptrtoint ptr %138 to i64
  %140 = icmp sgt i64 %137, 0
  br i1 %140, label %.lr.ph.preheader.i.i38, label %.preheader2.i.i32

.lr.ph.preheader.i.i38:                           ; preds = %136
  tail call void @llvm.memset.p0.i64(ptr align 1 %138, i8 0, i64 %137, i1 false)
  br label %.preheader2.i.i32

.preheader2.i.i32:                                ; preds = %.lr.ph.preheader.i.i38, %136
  %141 = icmp ult i64 %131, 9223372036854775807
  %.pre.i.i33 = load ptr, ptr %0, align 8
  br i1 %141, label %.lr.ph5.i.i36.preheader, label %.preheader.i.i34

.lr.ph5.i.i36.preheader:                          ; preds = %.preheader2.i.i32
  %.pre.i.i3399 = ptrtoint ptr %.pre.i.i33 to i64
  %min.iters.check103 = icmp ult i64 %132, 4
  %142 = sub i64 %139, %.pre.i.i3399
  %diff.check100 = icmp ult i64 %142, 4
  %or.cond116 = select i1 %min.iters.check103, i1 true, i1 %diff.check100
  br i1 %or.cond116, label %.lr.ph5.i.i36.preheader117, label %vector.ph104

.lr.ph5.i.i36.preheader117:                       ; preds = %middle.block101, %.lr.ph5.i.i36.preheader
  %.14.i.i37.ph = phi i64 [ 0, %.lr.ph5.i.i36.preheader ], [ %n.vec106, %middle.block101 ]
  br label %.lr.ph5.i.i36

vector.ph104:                                     ; preds = %.lr.ph5.i.i36.preheader
  %n.vec106 = and i64 %132, -4
  br label %vector.body108

vector.body108:                                   ; preds = %vector.body108, %vector.ph104
  %index109 = phi i64 [ 0, %vector.ph104 ], [ %index.next111, %vector.body108 ]
  %143 = getelementptr i8, ptr %138, i64 %index109
  %144 = getelementptr i8, ptr %.pre.i.i33, i64 %index109
  %wide.load110 = load <4 x i8>, ptr %144, align 1
  store <4 x i8> %wide.load110, ptr %143, align 1
  %index.next111 = add nuw i64 %index109, 4
  %145 = icmp eq i64 %index.next111, %n.vec106
  br i1 %145, label %middle.block101, label %vector.body108, !llvm.loop !31

middle.block101:                                  ; preds = %vector.body108
  %cmp.n112 = icmp eq i64 %132, %n.vec106
  br i1 %cmp.n112, label %.preheader.i.i34, label %.lr.ph5.i.i36.preheader117

.preheader.i.i34:                                 ; preds = %.lr.ph5.i.i36, %middle.block101, %.preheader2.i.i32
  tail call void @free(ptr %.pre.i.i33)
  store i64 %137, ptr %13, align 8
  store ptr %138, ptr %0, align 8
  %.pre.i35 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit41

.lr.ph5.i.i36:                                    ; preds = %.lr.ph5.i.i36.preheader117, %.lr.ph5.i.i36
  %.14.i.i37 = phi i64 [ %149, %.lr.ph5.i.i36 ], [ %.14.i.i37.ph, %.lr.ph5.i.i36.preheader117 ]
  %146 = getelementptr i8, ptr %138, i64 %.14.i.i37
  %147 = getelementptr i8, ptr %.pre.i.i33, i64 %.14.i.i37
  %148 = load i8, ptr %147, align 1
  store i8 %148, ptr %146, align 1
  %149 = add nuw nsw i64 %.14.i.i37, 1
  %150 = icmp slt i64 %149, %132
  br i1 %150, label %.lr.ph5.i.i36, label %.preheader.i.i34, !llvm.loop !32

rl_m_append__VectorTint8_tT_int8_t.exit41:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i39, %.preheader.i.i34
  %151 = phi ptr [ %.pre2.i40, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i39 ], [ %138, %.preheader.i.i34 ]
  %152 = phi i64 [ %132, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i39 ], [ %.pre.i35, %.preheader.i.i34 ]
  %153 = getelementptr i8, ptr %151, i64 %152
  store i8 0, ptr %153, align 1
  %154 = load i64, ptr %3, align 8
  %155 = add i64 %154, 1
  store i64 %155, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_append__String_String(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %4, 0
  br i1 %5, label %rl_m_pop__VectorTint8_tT_r_int8_t.exit, label %6

6:                                                ; preds = %2
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_7)
  tail call void @llvm.trap()
  unreachable

rl_m_pop__VectorTint8_tT_r_int8_t.exit:           ; preds = %2
  %8 = add nsw i64 %4, -1
  %9 = load ptr, ptr %0, align 8
  %10 = getelementptr i8, ptr %9, i64 %8
  store i64 %8, ptr %3, align 8
  store i8 0, ptr %10, align 1
  %11 = getelementptr i8, ptr %1, i64 8
  %12 = load i64, ptr %11, align 8
  %13 = add i64 %12, -1
  %14 = icmp sgt i64 %13, 0
  br i1 %14, label %.lr.ph, label %rl_m_pop__VectorTint8_tT_r_int8_t.exit.._crit_edge_crit_edge

rl_m_pop__VectorTint8_tT_r_int8_t.exit.._crit_edge_crit_edge: ; preds = %rl_m_pop__VectorTint8_tT_r_int8_t.exit
  %.pre = load i64, ptr %3, align 8
  br label %._crit_edge

.lr.ph:                                           ; preds = %rl_m_pop__VectorTint8_tT_r_int8_t.exit
  %15 = getelementptr i8, ptr %0, i64 16
  br label %16

16:                                               ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit, %.lr.ph
  %17 = phi i64 [ %12, %.lr.ph ], [ %49, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %storemerge15 = phi i64 [ 0, %.lr.ph ], [ %48, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %18 = icmp slt i64 %storemerge15, %17
  br i1 %18, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %19

19:                                               ; preds = %16
  %20 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %16
  %21 = load ptr, ptr %1, align 8
  %22 = getelementptr i8, ptr %21, i64 %storemerge15
  %23 = load i64, ptr %3, align 8
  %24 = add i64 %23, 1
  %25 = load i64, ptr %15, align 8
  %26 = icmp sgt i64 %25, %24
  br i1 %26, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, label %27

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i: ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %.pre2.i = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

27:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %28 = shl i64 %24, 1
  %29 = tail call ptr @malloc(i64 %28)
  %30 = ptrtoint ptr %29 to i64
  %31 = icmp sgt i64 %28, 0
  br i1 %31, label %.lr.ph.preheader.i.i, label %.preheader2.i.i

.lr.ph.preheader.i.i:                             ; preds = %27
  tail call void @llvm.memset.p0.i64(ptr align 1 %29, i8 0, i64 %28, i1 false)
  br label %.preheader2.i.i

.preheader2.i.i:                                  ; preds = %.lr.ph.preheader.i.i, %27
  %32 = icmp sgt i64 %23, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %32, label %.lr.ph5.i.i.preheader, label %.preheader.i.i

.lr.ph5.i.i.preheader:                            ; preds = %.preheader2.i.i
  %.pre.i.i16 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %23, 4
  %33 = sub i64 %30, %.pre.i.i16
  %diff.check = icmp ult i64 %33, 4
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph5.i.i.preheader34, label %vector.ph

.lr.ph5.i.i.preheader34:                          ; preds = %middle.block, %.lr.ph5.i.i.preheader
  %.14.i.i.ph = phi i64 [ 0, %.lr.ph5.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph5.i.i

vector.ph:                                        ; preds = %.lr.ph5.i.i.preheader
  %n.vec = and i64 %23, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %34 = getelementptr i8, ptr %29, i64 %index
  %35 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %wide.load = load <4 x i8>, ptr %35, align 1
  store <4 x i8> %wide.load, ptr %34, align 1
  %index.next = add nuw i64 %index, 4
  %36 = icmp eq i64 %index.next, %n.vec
  br i1 %36, label %middle.block, label %vector.body, !llvm.loop !33

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %23, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph5.i.i.preheader34

.preheader.i.i:                                   ; preds = %.lr.ph5.i.i, %middle.block, %.preheader2.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %28, ptr %15, align 8
  store ptr %29, ptr %0, align 8
  %.pre.i = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph5.i.i:                                      ; preds = %.lr.ph5.i.i.preheader34, %.lr.ph5.i.i
  %.14.i.i = phi i64 [ %40, %.lr.ph5.i.i ], [ %.14.i.i.ph, %.lr.ph5.i.i.preheader34 ]
  %37 = getelementptr i8, ptr %29, i64 %.14.i.i
  %38 = getelementptr i8, ptr %.pre.i.i, i64 %.14.i.i
  %39 = load i8, ptr %38, align 1
  store i8 %39, ptr %37, align 1
  %40 = add nuw nsw i64 %.14.i.i, 1
  %41 = icmp slt i64 %40, %23
  br i1 %41, label %.lr.ph5.i.i, label %.preheader.i.i, !llvm.loop !34

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %42 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %29, %.preheader.i.i ]
  %43 = phi i64 [ %23, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %44 = getelementptr i8, ptr %42, i64 %43
  %45 = load i8, ptr %22, align 1
  store i8 %45, ptr %44, align 1
  %46 = load i64, ptr %3, align 8
  %47 = add i64 %46, 1
  store i64 %47, ptr %3, align 8
  %48 = add nuw nsw i64 %storemerge15, 1
  %49 = load i64, ptr %11, align 8
  %50 = add i64 %49, -1
  %51 = icmp slt i64 %48, %50
  br i1 %51, label %16, label %._crit_edge

._crit_edge:                                      ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit, %rl_m_pop__VectorTint8_tT_r_int8_t.exit.._crit_edge_crit_edge
  %52 = phi i64 [ %.pre, %rl_m_pop__VectorTint8_tT_r_int8_t.exit.._crit_edge_crit_edge ], [ %47, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %53 = add i64 %52, 1
  %54 = getelementptr i8, ptr %0, i64 16
  %55 = load i64, ptr %54, align 8
  %56 = icmp sgt i64 %55, %53
  br i1 %56, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8, label %57

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8: ; preds = %._crit_edge
  %.pre2.i9 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit10

57:                                               ; preds = %._crit_edge
  %58 = shl i64 %53, 1
  %59 = tail call ptr @malloc(i64 %58)
  %60 = ptrtoint ptr %59 to i64
  %61 = icmp sgt i64 %58, 0
  br i1 %61, label %.lr.ph.preheader.i.i7, label %.preheader2.i.i1

.lr.ph.preheader.i.i7:                            ; preds = %57
  tail call void @llvm.memset.p0.i64(ptr align 1 %59, i8 0, i64 %58, i1 false)
  br label %.preheader2.i.i1

.preheader2.i.i1:                                 ; preds = %.lr.ph.preheader.i.i7, %57
  %62 = icmp sgt i64 %52, 0
  %.pre.i.i2 = load ptr, ptr %0, align 8
  br i1 %62, label %.lr.ph5.i.i5.preheader, label %.preheader.i.i3

.lr.ph5.i.i5.preheader:                           ; preds = %.preheader2.i.i1
  %.pre.i.i218 = ptrtoint ptr %.pre.i.i2 to i64
  %min.iters.check22 = icmp ult i64 %52, 4
  %63 = sub i64 %60, %.pre.i.i218
  %diff.check19 = icmp ult i64 %63, 4
  %or.cond32 = select i1 %min.iters.check22, i1 true, i1 %diff.check19
  br i1 %or.cond32, label %.lr.ph5.i.i5.preheader33, label %vector.ph23

.lr.ph5.i.i5.preheader33:                         ; preds = %middle.block20, %.lr.ph5.i.i5.preheader
  %.14.i.i6.ph = phi i64 [ 0, %.lr.ph5.i.i5.preheader ], [ %n.vec25, %middle.block20 ]
  br label %.lr.ph5.i.i5

vector.ph23:                                      ; preds = %.lr.ph5.i.i5.preheader
  %n.vec25 = and i64 %52, 9223372036854775804
  br label %vector.body27

vector.body27:                                    ; preds = %vector.body27, %vector.ph23
  %index28 = phi i64 [ 0, %vector.ph23 ], [ %index.next30, %vector.body27 ]
  %64 = getelementptr i8, ptr %59, i64 %index28
  %65 = getelementptr i8, ptr %.pre.i.i2, i64 %index28
  %wide.load29 = load <4 x i8>, ptr %65, align 1
  store <4 x i8> %wide.load29, ptr %64, align 1
  %index.next30 = add nuw i64 %index28, 4
  %66 = icmp eq i64 %index.next30, %n.vec25
  br i1 %66, label %middle.block20, label %vector.body27, !llvm.loop !35

middle.block20:                                   ; preds = %vector.body27
  %cmp.n31 = icmp eq i64 %52, %n.vec25
  br i1 %cmp.n31, label %.preheader.i.i3, label %.lr.ph5.i.i5.preheader33

.preheader.i.i3:                                  ; preds = %.lr.ph5.i.i5, %middle.block20, %.preheader2.i.i1
  tail call void @free(ptr %.pre.i.i2)
  store i64 %58, ptr %54, align 8
  store ptr %59, ptr %0, align 8
  %.pre.i4 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit10

.lr.ph5.i.i5:                                     ; preds = %.lr.ph5.i.i5.preheader33, %.lr.ph5.i.i5
  %.14.i.i6 = phi i64 [ %70, %.lr.ph5.i.i5 ], [ %.14.i.i6.ph, %.lr.ph5.i.i5.preheader33 ]
  %67 = getelementptr i8, ptr %59, i64 %.14.i.i6
  %68 = getelementptr i8, ptr %.pre.i.i2, i64 %.14.i.i6
  %69 = load i8, ptr %68, align 1
  store i8 %69, ptr %67, align 1
  %70 = add nuw nsw i64 %.14.i.i6, 1
  %71 = icmp slt i64 %70, %52
  br i1 %71, label %.lr.ph5.i.i5, label %.preheader.i.i3, !llvm.loop !36

rl_m_append__VectorTint8_tT_int8_t.exit10:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8, %.preheader.i.i3
  %72 = phi ptr [ %.pre2.i9, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8 ], [ %59, %.preheader.i.i3 ]
  %73 = phi i64 [ %52, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8 ], [ %.pre.i4, %.preheader.i.i3 ]
  %74 = getelementptr i8, ptr %72, i64 %73
  store i8 0, ptr %74, align 1
  %75 = load i64, ptr %3, align 8
  %76 = add i64 %75, 1
  store i64 %76, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_append__String_strlit(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %4, 0
  br i1 %5, label %rl_m_pop__VectorTint8_tT_r_int8_t.exit, label %6

6:                                                ; preds = %2
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_7)
  tail call void @llvm.trap()
  unreachable

rl_m_pop__VectorTint8_tT_r_int8_t.exit:           ; preds = %2
  %8 = add nsw i64 %4, -1
  %9 = load ptr, ptr %0, align 8
  %10 = getelementptr i8, ptr %9, i64 %8
  store i64 %8, ptr %3, align 8
  store i8 0, ptr %10, align 1
  %11 = load ptr, ptr %1, align 8
  %12 = load i8, ptr %11, align 1
  %.not11 = icmp eq i8 %12, 0
  %.pre14 = load i64, ptr %3, align 8
  br i1 %.not11, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %rl_m_pop__VectorTint8_tT_r_int8_t.exit
  %13 = getelementptr i8, ptr %0, i64 16
  br label %14

14:                                               ; preds = %.lr.ph, %rl_m_append__VectorTint8_tT_int8_t.exit
  %15 = phi i8 [ %12, %.lr.ph ], [ %45, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %16 = phi i64 [ %.pre14, %.lr.ph ], [ %41, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %17 = phi ptr [ %11, %.lr.ph ], [ %44, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %.012 = phi i64 [ 0, %.lr.ph ], [ %42, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %18 = add i64 %16, 1
  %19 = load i64, ptr %13, align 8
  %20 = icmp sgt i64 %19, %18
  br i1 %20, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, label %21

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i: ; preds = %14
  %.pre2.i = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

21:                                               ; preds = %14
  %22 = shl i64 %18, 1
  %23 = tail call ptr @malloc(i64 %22)
  %24 = ptrtoint ptr %23 to i64
  %25 = icmp sgt i64 %22, 0
  br i1 %25, label %.lr.ph.preheader.i.i, label %.preheader2.i.i

.lr.ph.preheader.i.i:                             ; preds = %21
  tail call void @llvm.memset.p0.i64(ptr align 1 %23, i8 0, i64 %22, i1 false)
  br label %.preheader2.i.i

.preheader2.i.i:                                  ; preds = %.lr.ph.preheader.i.i, %21
  %26 = icmp sgt i64 %16, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %26, label %.lr.ph5.i.i.preheader, label %.preheader.i.i

.lr.ph5.i.i.preheader:                            ; preds = %.preheader2.i.i
  %.pre.i.i15 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %16, 4
  %27 = sub i64 %24, %.pre.i.i15
  %diff.check = icmp ult i64 %27, 4
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph5.i.i.preheader33, label %vector.ph

.lr.ph5.i.i.preheader33:                          ; preds = %middle.block, %.lr.ph5.i.i.preheader
  %.14.i.i.ph = phi i64 [ 0, %.lr.ph5.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph5.i.i

vector.ph:                                        ; preds = %.lr.ph5.i.i.preheader
  %n.vec = and i64 %16, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %28 = getelementptr i8, ptr %23, i64 %index
  %29 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %wide.load = load <4 x i8>, ptr %29, align 1
  store <4 x i8> %wide.load, ptr %28, align 1
  %index.next = add nuw i64 %index, 4
  %30 = icmp eq i64 %index.next, %n.vec
  br i1 %30, label %middle.block, label %vector.body, !llvm.loop !37

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %16, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph5.i.i.preheader33

.preheader.i.i:                                   ; preds = %.lr.ph5.i.i, %middle.block, %.preheader2.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %22, ptr %13, align 8
  store ptr %23, ptr %0, align 8
  %.pre.i = load i64, ptr %3, align 8
  %.pre13 = load i8, ptr %17, align 1
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph5.i.i:                                      ; preds = %.lr.ph5.i.i.preheader33, %.lr.ph5.i.i
  %.14.i.i = phi i64 [ %34, %.lr.ph5.i.i ], [ %.14.i.i.ph, %.lr.ph5.i.i.preheader33 ]
  %31 = getelementptr i8, ptr %23, i64 %.14.i.i
  %32 = getelementptr i8, ptr %.pre.i.i, i64 %.14.i.i
  %33 = load i8, ptr %32, align 1
  store i8 %33, ptr %31, align 1
  %34 = add nuw nsw i64 %.14.i.i, 1
  %35 = icmp slt i64 %34, %16
  br i1 %35, label %.lr.ph5.i.i, label %.preheader.i.i, !llvm.loop !38

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %36 = phi i8 [ %15, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre13, %.preheader.i.i ]
  %37 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %23, %.preheader.i.i ]
  %38 = phi i64 [ %16, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %39 = getelementptr i8, ptr %37, i64 %38
  store i8 %36, ptr %39, align 1
  %40 = load i64, ptr %3, align 8
  %41 = add i64 %40, 1
  store i64 %41, ptr %3, align 8
  %42 = add i64 %.012, 1
  %43 = load ptr, ptr %1, align 8
  %44 = getelementptr i8, ptr %43, i64 %42
  %45 = load i8, ptr %44, align 1
  %.not = icmp eq i8 %45, 0
  br i1 %.not, label %._crit_edge, label %14

._crit_edge:                                      ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit, %rl_m_pop__VectorTint8_tT_r_int8_t.exit
  %46 = phi i64 [ %.pre14, %rl_m_pop__VectorTint8_tT_r_int8_t.exit ], [ %41, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %47 = add i64 %46, 1
  %48 = getelementptr i8, ptr %0, i64 16
  %49 = load i64, ptr %48, align 8
  %50 = icmp sgt i64 %49, %47
  br i1 %50, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8, label %51

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8: ; preds = %._crit_edge
  %.pre2.i9 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit10

51:                                               ; preds = %._crit_edge
  %52 = shl i64 %47, 1
  %53 = tail call ptr @malloc(i64 %52)
  %54 = ptrtoint ptr %53 to i64
  %55 = icmp sgt i64 %52, 0
  br i1 %55, label %.lr.ph.preheader.i.i7, label %.preheader2.i.i1

.lr.ph.preheader.i.i7:                            ; preds = %51
  tail call void @llvm.memset.p0.i64(ptr align 1 %53, i8 0, i64 %52, i1 false)
  br label %.preheader2.i.i1

.preheader2.i.i1:                                 ; preds = %.lr.ph.preheader.i.i7, %51
  %56 = icmp sgt i64 %46, 0
  %.pre.i.i2 = load ptr, ptr %0, align 8
  br i1 %56, label %.lr.ph5.i.i5.preheader, label %.preheader.i.i3

.lr.ph5.i.i5.preheader:                           ; preds = %.preheader2.i.i1
  %.pre.i.i217 = ptrtoint ptr %.pre.i.i2 to i64
  %min.iters.check21 = icmp ult i64 %46, 4
  %57 = sub i64 %54, %.pre.i.i217
  %diff.check18 = icmp ult i64 %57, 4
  %or.cond31 = select i1 %min.iters.check21, i1 true, i1 %diff.check18
  br i1 %or.cond31, label %.lr.ph5.i.i5.preheader32, label %vector.ph22

.lr.ph5.i.i5.preheader32:                         ; preds = %middle.block19, %.lr.ph5.i.i5.preheader
  %.14.i.i6.ph = phi i64 [ 0, %.lr.ph5.i.i5.preheader ], [ %n.vec24, %middle.block19 ]
  br label %.lr.ph5.i.i5

vector.ph22:                                      ; preds = %.lr.ph5.i.i5.preheader
  %n.vec24 = and i64 %46, 9223372036854775804
  br label %vector.body26

vector.body26:                                    ; preds = %vector.body26, %vector.ph22
  %index27 = phi i64 [ 0, %vector.ph22 ], [ %index.next29, %vector.body26 ]
  %58 = getelementptr i8, ptr %53, i64 %index27
  %59 = getelementptr i8, ptr %.pre.i.i2, i64 %index27
  %wide.load28 = load <4 x i8>, ptr %59, align 1
  store <4 x i8> %wide.load28, ptr %58, align 1
  %index.next29 = add nuw i64 %index27, 4
  %60 = icmp eq i64 %index.next29, %n.vec24
  br i1 %60, label %middle.block19, label %vector.body26, !llvm.loop !39

middle.block19:                                   ; preds = %vector.body26
  %cmp.n30 = icmp eq i64 %46, %n.vec24
  br i1 %cmp.n30, label %.preheader.i.i3, label %.lr.ph5.i.i5.preheader32

.preheader.i.i3:                                  ; preds = %.lr.ph5.i.i5, %middle.block19, %.preheader2.i.i1
  tail call void @free(ptr %.pre.i.i2)
  store i64 %52, ptr %48, align 8
  store ptr %53, ptr %0, align 8
  %.pre.i4 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit10

.lr.ph5.i.i5:                                     ; preds = %.lr.ph5.i.i5.preheader32, %.lr.ph5.i.i5
  %.14.i.i6 = phi i64 [ %64, %.lr.ph5.i.i5 ], [ %.14.i.i6.ph, %.lr.ph5.i.i5.preheader32 ]
  %61 = getelementptr i8, ptr %53, i64 %.14.i.i6
  %62 = getelementptr i8, ptr %.pre.i.i2, i64 %.14.i.i6
  %63 = load i8, ptr %62, align 1
  store i8 %63, ptr %61, align 1
  %64 = add nuw nsw i64 %.14.i.i6, 1
  %65 = icmp slt i64 %64, %46
  br i1 %65, label %.lr.ph5.i.i5, label %.preheader.i.i3, !llvm.loop !40

rl_m_append__VectorTint8_tT_int8_t.exit10:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8, %.preheader.i.i3
  %66 = phi ptr [ %.pre2.i9, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8 ], [ %53, %.preheader.i.i3 ]
  %67 = phi i64 [ %46, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8 ], [ %.pre.i4, %.preheader.i.i3 ]
  %68 = getelementptr i8, ptr %66, i64 %67
  store i8 0, ptr %68, align 1
  %69 = load i64, ptr %3, align 8
  %70 = add i64 %69, 1
  store i64 %70, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_count__String_int8_t_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = getelementptr i8, ptr %1, i64 8
  %5 = load i64, ptr %4, align 8
  %6 = add i64 %5, -1
  %.not5 = icmp eq i64 %6, 0
  br i1 %.not5, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %3
  %smax = tail call i64 @llvm.smax.i64(i64 %5, i64 0)
  %7 = add i64 %5, -2
  %.not9.not = icmp ugt i64 %smax, %7
  br i1 %.not9.not, label %.lr.ph.preheader.split, label %13

.lr.ph.preheader.split:                           ; preds = %.lr.ph.preheader
  %.pre = load ptr, ptr %1, align 8
  %.pre8 = load i8, ptr %2, align 1
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader.split, %.lr.ph
  %.07 = phi i64 [ %.1, %.lr.ph ], [ 0, %.lr.ph.preheader.split ]
  %storemerge6 = phi i64 [ %12, %.lr.ph ], [ 0, %.lr.ph.preheader.split ]
  %8 = getelementptr i8, ptr %.pre, i64 %storemerge6
  %9 = load i8, ptr %8, align 1
  %10 = icmp eq i8 %9, %.pre8
  %11 = zext i1 %10 to i64
  %.1 = add i64 %.07, %11
  %12 = add nuw nsw i64 %storemerge6, 1
  %.not = icmp eq i64 %12, %6
  br i1 %.not, label %._crit_edge, label %.lr.ph

13:                                               ; preds = %.lr.ph.preheader
  %14 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

._crit_edge:                                      ; preds = %.lr.ph, %3
  %.0.lcssa = phi i64 [ 0, %3 ], [ %.1, %.lr.ph ]
  store i64 %.0.lcssa, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_m_size__String_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = getelementptr i8, ptr %1, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = add i64 %4, -1
  store i64 %5, ptr %0, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_substring_matches__String_strlit_int64_t_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2, ptr nocapture readonly %3) local_unnamed_addr #5 {
  %5 = getelementptr i8, ptr %1, i64 8
  %6 = load i64, ptr %5, align 8
  %7 = add i64 %6, -1
  %8 = load i64, ptr %3, align 8
  %.not = icmp slt i64 %8, %7
  br i1 %.not, label %.preheader, label %common.ret

.preheader:                                       ; preds = %4
  %9 = load ptr, ptr %2, align 8
  %10 = load i8, ptr %9, align 1
  %.not36 = icmp eq i8 %10, 0
  br i1 %.not36, label %common.ret, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %.preheader
  %11 = icmp sgt i64 %8, -1
  br label %.lr.ph

12:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %13 = add i64 %.07, 1
  %14 = getelementptr i8, ptr %9, i64 %13
  %15 = load i8, ptr %14, align 1
  %.not3 = icmp eq i8 %15, 0
  br i1 %.not3, label %common.ret, label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %12
  %16 = phi i8 [ %15, %12 ], [ %10, %.lr.ph.preheader ]
  %.07 = phi i64 [ %13, %12 ], [ 0, %.lr.ph.preheader ]
  %17 = add nuw i64 %.07, %8
  br i1 %11, label %20, label %18

18:                                               ; preds = %.lr.ph
  %19 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

20:                                               ; preds = %.lr.ph
  %21 = icmp slt i64 %17, %6
  br i1 %21, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %22

22:                                               ; preds = %20
  %23 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %20
  %24 = load ptr, ptr %1, align 8
  %25 = getelementptr i8, ptr %24, i64 %17
  %26 = load i8, ptr %25, align 1
  %.not4 = icmp eq i8 %16, %26
  br i1 %.not4, label %12, label %common.ret

common.ret:                                       ; preds = %12, %rl_m_get__String_int64_t_r_int8_tRef.exit, %4, %.preheader
  %.sink = phi i8 [ 1, %.preheader ], [ 0, %4 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit ], [ 1, %12 ]
  store i8 %.sink, ptr %0, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_get__String_int64_t_r_int8_tRef(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = load i64, ptr %2, align 8
  %5 = icmp sgt i64 %4, -1
  br i1 %5, label %8, label %6

6:                                                ; preds = %3
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

8:                                                ; preds = %3
  %9 = getelementptr i8, ptr %1, i64 8
  %10 = load i64, ptr %9, align 8
  %11 = icmp slt i64 %4, %10
  br i1 %11, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit, label %12

12:                                               ; preds = %8
  %13 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit: ; preds = %8
  %14 = load ptr, ptr %1, align 8
  %15 = getelementptr i8, ptr %14, i64 %4
  store ptr %15, ptr %0, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_append__String_int8_t(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %4, 0
  br i1 %5, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit, label %6

6:                                                ; preds = %2
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit:       ; preds = %2
  %8 = load ptr, ptr %0, align 8
  %9 = getelementptr i8, ptr %8, i64 %4
  %10 = getelementptr i8, ptr %9, i64 -1
  %11 = load i8, ptr %1, align 1
  store i8 %11, ptr %10, align 1
  %12 = load i64, ptr %3, align 8
  %13 = add i64 %12, 1
  %14 = getelementptr i8, ptr %0, i64 16
  %15 = load i64, ptr %14, align 8
  %16 = icmp sgt i64 %15, %13
  br i1 %16, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, label %17

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit
  %.pre2.i = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

17:                                               ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit
  %18 = shl i64 %13, 1
  %19 = tail call ptr @malloc(i64 %18)
  %20 = ptrtoint ptr %19 to i64
  %21 = icmp sgt i64 %18, 0
  br i1 %21, label %.lr.ph.preheader.i.i, label %.preheader2.i.i

.lr.ph.preheader.i.i:                             ; preds = %17
  tail call void @llvm.memset.p0.i64(ptr align 1 %19, i8 0, i64 %18, i1 false)
  br label %.preheader2.i.i

.preheader2.i.i:                                  ; preds = %.lr.ph.preheader.i.i, %17
  %22 = icmp sgt i64 %12, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %22, label %.lr.ph5.i.i.preheader, label %.preheader.i.i

.lr.ph5.i.i.preheader:                            ; preds = %.preheader2.i.i
  %.pre.i.i1 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %12, 4
  %23 = sub i64 %20, %.pre.i.i1
  %diff.check = icmp ult i64 %23, 4
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph5.i.i.preheader2, label %vector.ph

.lr.ph5.i.i.preheader2:                           ; preds = %middle.block, %.lr.ph5.i.i.preheader
  %.14.i.i.ph = phi i64 [ 0, %.lr.ph5.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph5.i.i

vector.ph:                                        ; preds = %.lr.ph5.i.i.preheader
  %n.vec = and i64 %12, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %24 = getelementptr i8, ptr %19, i64 %index
  %25 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %wide.load = load <4 x i8>, ptr %25, align 1
  store <4 x i8> %wide.load, ptr %24, align 1
  %index.next = add nuw i64 %index, 4
  %26 = icmp eq i64 %index.next, %n.vec
  br i1 %26, label %middle.block, label %vector.body, !llvm.loop !41

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %12, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph5.i.i.preheader2

.preheader.i.i:                                   ; preds = %.lr.ph5.i.i, %middle.block, %.preheader2.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %18, ptr %14, align 8
  store ptr %19, ptr %0, align 8
  %.pre.i = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph5.i.i:                                      ; preds = %.lr.ph5.i.i.preheader2, %.lr.ph5.i.i
  %.14.i.i = phi i64 [ %30, %.lr.ph5.i.i ], [ %.14.i.i.ph, %.lr.ph5.i.i.preheader2 ]
  %27 = getelementptr i8, ptr %19, i64 %.14.i.i
  %28 = getelementptr i8, ptr %.pre.i.i, i64 %.14.i.i
  %29 = load i8, ptr %28, align 1
  store i8 %29, ptr %27, align 1
  %30 = add nuw nsw i64 %.14.i.i, 1
  %31 = icmp slt i64 %30, %12
  br i1 %31, label %.lr.ph5.i.i, label %.preheader.i.i, !llvm.loop !42

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %32 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %19, %.preheader.i.i ]
  %33 = phi i64 [ %12, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %34 = getelementptr i8, ptr %32, i64 %33
  store i8 0, ptr %34, align 1
  %35 = load i64, ptr %3, align 8
  %36 = add i64 %35, 1
  store i64 %36, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_init__String(ptr nocapture %0) local_unnamed_addr #5 {
  %2 = getelementptr i8, ptr %0, i64 8
  store i64 0, ptr %2, align 8
  %3 = getelementptr i8, ptr %0, i64 16
  store i64 4, ptr %3, align 8
  %4 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %4, ptr %0, align 8
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i, %1
  %.01.i = phi i64 [ %7, %.lr.ph.i ], [ 0, %1 ]
  %5 = load ptr, ptr %0, align 8
  %6 = getelementptr i8, ptr %5, i64 %.01.i
  store i8 0, ptr %6, align 1
  %7 = add nuw nsw i64 %.01.i, 1
  %8 = load i64, ptr %3, align 8
  %9 = icmp slt i64 %7, %8
  br i1 %9, label %.lr.ph.i, label %rl_m_init__VectorTint8_tT.exit

rl_m_init__VectorTint8_tT.exit:                   ; preds = %.lr.ph.i
  %10 = load i64, ptr %2, align 8
  %11 = add i64 %10, 1
  %12 = icmp sgt i64 %8, %11
  br i1 %12, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, label %13

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i: ; preds = %rl_m_init__VectorTint8_tT.exit
  %.pre2.i = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

13:                                               ; preds = %rl_m_init__VectorTint8_tT.exit
  %14 = shl i64 %11, 1
  %15 = tail call ptr @malloc(i64 %14)
  %16 = ptrtoint ptr %15 to i64
  %17 = icmp sgt i64 %14, 0
  br i1 %17, label %.lr.ph.preheader.i.i, label %.preheader2.i.i

.lr.ph.preheader.i.i:                             ; preds = %13
  tail call void @llvm.memset.p0.i64(ptr align 1 %15, i8 0, i64 %14, i1 false)
  br label %.preheader2.i.i

.preheader2.i.i:                                  ; preds = %.lr.ph.preheader.i.i, %13
  %18 = icmp sgt i64 %10, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %18, label %.lr.ph5.i.i.preheader, label %.preheader.i.i

.lr.ph5.i.i.preheader:                            ; preds = %.preheader2.i.i
  %.pre.i.i1 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %10, 4
  %19 = sub i64 %16, %.pre.i.i1
  %diff.check = icmp ult i64 %19, 4
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph5.i.i.preheader2, label %vector.ph

.lr.ph5.i.i.preheader2:                           ; preds = %middle.block, %.lr.ph5.i.i.preheader
  %.14.i.i.ph = phi i64 [ 0, %.lr.ph5.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph5.i.i

vector.ph:                                        ; preds = %.lr.ph5.i.i.preheader
  %n.vec = and i64 %10, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %20 = getelementptr i8, ptr %15, i64 %index
  %21 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %wide.load = load <4 x i8>, ptr %21, align 1
  store <4 x i8> %wide.load, ptr %20, align 1
  %index.next = add nuw i64 %index, 4
  %22 = icmp eq i64 %index.next, %n.vec
  br i1 %22, label %middle.block, label %vector.body, !llvm.loop !43

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %10, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph5.i.i.preheader2

.preheader.i.i:                                   ; preds = %.lr.ph5.i.i, %middle.block, %.preheader2.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %14, ptr %3, align 8
  store ptr %15, ptr %0, align 8
  %.pre.i = load i64, ptr %2, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph5.i.i:                                      ; preds = %.lr.ph5.i.i.preheader2, %.lr.ph5.i.i
  %.14.i.i = phi i64 [ %26, %.lr.ph5.i.i ], [ %.14.i.i.ph, %.lr.ph5.i.i.preheader2 ]
  %23 = getelementptr i8, ptr %15, i64 %.14.i.i
  %24 = getelementptr i8, ptr %.pre.i.i, i64 %.14.i.i
  %25 = load i8, ptr %24, align 1
  store i8 %25, ptr %23, align 1
  %26 = add nuw nsw i64 %.14.i.i, 1
  %27 = icmp slt i64 %26, %10
  br i1 %27, label %.lr.ph5.i.i, label %.preheader.i.i, !llvm.loop !44

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %28 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %15, %.preheader.i.i ]
  %29 = phi i64 [ %10, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %30 = getelementptr i8, ptr %28, i64 %29
  store i8 0, ptr %30, align 1
  %31 = load i64, ptr %2, align 8
  %32 = add i64 %31, 1
  store i64 %32, ptr %2, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_s__strlit_r_String(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
rl_m_init__String.exit:
  %2 = alloca %String, align 8
  %3 = alloca %String, align 8
  %4 = getelementptr inbounds i8, ptr %3, i64 8
  %5 = getelementptr inbounds i8, ptr %3, i64 16
  store i64 4, ptr %5, align 8
  %6 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %6, ptr %3, align 8
  store i32 0, ptr %6, align 1
  store i64 1, ptr %4, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %3, ptr %1)
  %7 = getelementptr inbounds i8, ptr %2, i64 8
  %8 = getelementptr inbounds i8, ptr %2, i64 16
  store i64 4, ptr %8, align 8
  %9 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %9, ptr %2, align 8
  store i32 0, ptr %9, align 1
  store i64 1, ptr %7, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %2, ptr nonnull readonly %3)
  %10 = load i64, ptr %5, align 8
  %.not2.i.i = icmp eq i64 %10, 0
  br i1 %.not2.i.i, label %rl_m_drop__String.exit, label %11

11:                                               ; preds = %rl_m_init__String.exit
  %12 = load ptr, ptr %3, align 8
  tail call void @free(ptr %12)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %rl_m_init__String.exit, %11
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 1 dereferenceable(24) %0, ptr noundef nonnull align 8 dereferenceable(24) %2, i64 24, i1 false)
  ret void
}

; Function Attrs: nounwind
define void @rl_append_to_string__strlit_String(ptr nocapture readonly %0, ptr nocapture %1) local_unnamed_addr #5 {
  tail call void @rl_m_append__String_strlit(ptr %1, ptr %0)
  ret void
}

declare void @rl_append_to_string__int64_t_String(ptr, ptr) local_unnamed_addr

; Function Attrs: nounwind
define void @rl_append_to_string__String_String(ptr nocapture readonly %0, ptr nocapture %1) local_unnamed_addr #5 {
  tail call void @rl_m_append_quoted__String_String(ptr %1, ptr %0)
  ret void
}

; Function Attrs: nounwind
define void @rl_append_to_string__bool_String(ptr nocapture readonly %0, ptr nocapture %1) local_unnamed_addr #5 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = load i8, ptr %0, align 1
  %.not = icmp eq i8 %5, 0
  br i1 %.not, label %6, label %7

6:                                                ; preds = %2
  store ptr @str_12, ptr %4, align 8
  br label %8

7:                                                ; preds = %2
  store ptr @str_13, ptr %3, align 8
  br label %8

8:                                                ; preds = %7, %6
  %.sink = phi ptr [ %3, %7 ], [ %4, %6 ]
  call void @rl_m_append__String_strlit(ptr %1, ptr nonnull %.sink)
  ret void
}

define void @rl_to_string__int64_t_r_String(ptr nocapture writeonly %0, ptr %1) local_unnamed_addr {
vector.ph:
  %2 = alloca %String, align 8
  %3 = alloca %String, align 8
  %4 = getelementptr inbounds i8, ptr %3, i64 8
  %5 = getelementptr inbounds i8, ptr %3, i64 16
  store i64 4, ptr %5, align 8
  %6 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %6, ptr %3, align 8
  store <4 x i8> zeroinitializer, ptr %6, align 1
  store i8 0, ptr %6, align 1
  store i64 1, ptr %4, align 8
  call void @rl_append_to_string__int64_t_String(ptr %1, ptr nonnull %3)
  %7 = getelementptr inbounds i8, ptr %2, i64 8
  %8 = getelementptr inbounds i8, ptr %2, i64 16
  store i64 4, ptr %8, align 8
  %9 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %9, ptr %2, align 8
  store i32 0, ptr %9, align 1
  store i64 1, ptr %7, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %2, ptr nonnull readonly %3)
  %10 = load i64, ptr %5, align 8
  %.not2.i.i = icmp eq i64 %10, 0
  br i1 %.not2.i.i, label %rl_m_drop__String.exit, label %11

11:                                               ; preds = %vector.ph
  %12 = load ptr, ptr %3, align 8
  call void @free(ptr %12)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %vector.ph, %11
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 1 dereferenceable(24) %0, ptr noundef nonnull align 8 dereferenceable(24) %2, i64 24, i1 false)
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_is_space__int8_t_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i8, ptr %1, align 1
  switch i8 %3, label %4 [
    i8 32, label %common.ret
    i8 10, label %common.ret
  ]

common.ret:                                       ; preds = %2, %2, %4
  %.sink = phi i8 [ %6, %4 ], [ 1, %2 ], [ 1, %2 ]
  store i8 %.sink, ptr %0, align 1
  ret void

4:                                                ; preds = %2
  %5 = icmp eq i8 %3, 0
  %6 = zext i1 %5 to i8
  br label %common.ret
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_is_open_paren__int8_t_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i8, ptr %1, align 1
  switch i8 %3, label %4 [
    i8 40, label %common.ret
    i8 91, label %common.ret
  ]

common.ret:                                       ; preds = %2, %2, %4
  %.sink = phi i8 [ %6, %4 ], [ 1, %2 ], [ 1, %2 ]
  store i8 %.sink, ptr %0, align 1
  ret void

4:                                                ; preds = %2
  %5 = icmp eq i8 %3, 123
  %6 = zext i1 %5 to i8
  br label %common.ret
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_is_close_paren__int8_t_r_bool(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %3 = load i8, ptr %1, align 1
  switch i8 %3, label %4 [
    i8 41, label %common.ret
    i8 125, label %common.ret
  ]

common.ret:                                       ; preds = %2, %2, %4
  %.sink = phi i8 [ %6, %4 ], [ 1, %2 ], [ 1, %2 ]
  store i8 %.sink, ptr %0, align 1
  ret void

4:                                                ; preds = %2
  %5 = icmp eq i8 %3, 93
  %6 = zext i1 %5 to i8
  br label %common.ret
}

; Function Attrs: nofree norecurse nosync nounwind memory(read, argmem: readwrite, inaccessiblemem: none)
define void @rl_length__strlit_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #9 {
  %3 = load ptr, ptr %1, align 8
  br label %4

4:                                                ; preds = %4, %2
  %.0 = phi i64 [ 0, %2 ], [ %7, %4 ]
  %5 = getelementptr i8, ptr %3, i64 %.0
  %6 = load i8, ptr %5, align 1
  %.not = icmp eq i8 %6, 0
  %7 = add i64 %.0, 1
  br i1 %.not, label %8, label %4

8:                                                ; preds = %4
  store i64 %.0, ptr %0, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_parse_string__String_String_int64_t_r_bool(ptr nocapture writeonly %0, ptr nocapture %1, ptr nocapture readonly %2, ptr nocapture %3) local_unnamed_addr #5 {
  %5 = alloca %String, align 8
  %6 = alloca %String, align 8
  %7 = alloca %String, align 8
  %8 = alloca ptr, align 8
  store ptr @str_14, ptr %8, align 8
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %5)
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %6)
  %9 = getelementptr inbounds i8, ptr %6, i64 8
  %10 = getelementptr inbounds i8, ptr %6, i64 16
  store i64 4, ptr %10, align 8
  %11 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %11, ptr %6, align 8
  store i32 0, ptr %11, align 1
  store i64 1, ptr %9, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %6, ptr nonnull readonly %8)
  %12 = getelementptr inbounds i8, ptr %5, i64 8
  %13 = getelementptr inbounds i8, ptr %5, i64 16
  store i64 4, ptr %13, align 8
  %14 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %14, ptr %5, align 8
  store i32 0, ptr %14, align 1
  store i64 1, ptr %12, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %5, ptr nonnull readonly %6)
  %15 = load i64, ptr %10, align 8
  %.not2.i.i.i = icmp eq i64 %15, 0
  br i1 %.not2.i.i.i, label %rl_s__strlit_r_String.exit, label %16

16:                                               ; preds = %4
  %17 = load ptr, ptr %6, align 8
  tail call void @free(ptr %17)
  br label %rl_s__strlit_r_String.exit

rl_s__strlit_r_String.exit:                       ; preds = %4, %16
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %7, ptr noundef nonnull align 8 dereferenceable(24) %5, i64 24, i1 false)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %5)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %6)
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr %1, ptr nonnull readonly %7)
  %18 = getelementptr inbounds i8, ptr %7, i64 16
  %19 = load i64, ptr %18, align 8
  %.not2.i.i = icmp eq i64 %19, 0
  br i1 %.not2.i.i, label %rl_m_drop__String.exit, label %20

20:                                               ; preds = %rl_s__strlit_r_String.exit
  %21 = load ptr, ptr %7, align 8
  tail call void @free(ptr %21)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %rl_s__strlit_r_String.exit, %20
  %.pr.i = load i64, ptr %3, align 8
  %22 = icmp sgt i64 %.pr.i, -1
  br i1 %22, label %.lr.ph.i, label %._crit_edge.i

.lr.ph.i:                                         ; preds = %rl_m_drop__String.exit
  %23 = getelementptr i8, ptr %2, i64 8
  %24 = load i64, ptr %23, align 8
  %25 = icmp slt i64 %.pr.i, %24
  br i1 %25, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i, label %._crit_edge

._crit_edge.i:                                    ; preds = %rl_m_drop__String.exit
  %26 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

._crit_edge:                                      ; preds = %34, %.lr.ph.i
  %27 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit.i:      ; preds = %.lr.ph.i, %34
  %28 = phi i64 [ %36, %34 ], [ %24, %.lr.ph.i ]
  %29 = phi i64 [ %35, %34 ], [ %.pr.i, %.lr.ph.i ]
  %30 = load ptr, ptr %2, align 8
  %31 = getelementptr i8, ptr %30, i64 %29
  %32 = load i8, ptr %31, align 1
  switch i8 %32, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge [
    i8 32, label %rl_is_space__int8_t_r_bool.exit.thread.i
    i8 10, label %rl_is_space__int8_t_r_bool.exit.thread.i
    i8 0, label %rl_is_space__int8_t_r_bool.exit.thread.i
  ]

rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge: ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i
  %.pre = add nsw i64 %28, -1
  br label %rl__consume_space__String_int64_t.exit

rl_is_space__int8_t_r_bool.exit.thread.i:         ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i, %rl_m_get__String_int64_t_r_int8_tRef.exit.i, %rl_m_get__String_int64_t_r_int8_tRef.exit.i
  %33 = add nsw i64 %28, -1
  %.not8.i = icmp ult i64 %29, %33
  br i1 %.not8.i, label %34, label %rl__consume_space__String_int64_t.exit

34:                                               ; preds = %rl_is_space__int8_t_r_bool.exit.thread.i
  %35 = add nuw nsw i64 %29, 1
  store i64 %35, ptr %3, align 8
  %36 = load i64, ptr %23, align 8
  %37 = icmp slt i64 %35, %36
  br i1 %37, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i, label %._crit_edge

rl__consume_space__String_int64_t.exit:           ; preds = %rl_is_space__int8_t_r_bool.exit.thread.i, %rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge
  %.pre-phi = phi i64 [ %.pre, %rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge ], [ %33, %rl_is_space__int8_t_r_bool.exit.thread.i ]
  %.not.i = icmp slt i64 %29, %.pre-phi
  br i1 %.not.i, label %.lr.ph.preheader.i, label %common.ret

.lr.ph.preheader.i:                               ; preds = %rl__consume_space__String_int64_t.exit
  %38 = icmp sgt i64 %29, -1
  br i1 %38, label %.lr.ph.i6.preheader, label %43

.lr.ph.i6.preheader:                              ; preds = %.lr.ph.preheader.i
  %39 = icmp slt i64 %29, %28
  br i1 %39, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i7, label %45

rl_m_substring_matches__String_strlit_int64_t_r_bool.exit: ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i7
  %40 = add nuw nsw i64 %29, 1
  store i64 %40, ptr %3, align 8
  %41 = load i64, ptr %23, align 8
  %42 = add i64 %41, -2
  %.not531 = icmp eq i64 %29, %42
  br i1 %.not531, label %common.ret, label %.lr.ph

43:                                               ; preds = %.lr.ph.preheader.i
  %44 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

45:                                               ; preds = %.lr.ph.i6.preheader
  %46 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit.i7:     ; preds = %.lr.ph.i6.preheader
  %.not4.i = icmp eq i8 %32, 34
  br i1 %.not4.i, label %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit, label %common.ret

common.ret:                                       ; preds = %.backedge, %62, %rl__consume_space__String_int64_t.exit, %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit.i7, %117
  %.sink = phi i8 [ 1, %117 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i7 ], [ 0, %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit ], [ 0, %rl__consume_space__String_int64_t.exit ], [ 0, %62 ], [ 0, %.backedge ]
  store i8 %.sink, ptr %0, align 1
  ret void

.lr.ph:                                           ; preds = %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit
  %47 = getelementptr i8, ptr %1, i64 8
  %48 = getelementptr i8, ptr %1, i64 16
  br label %49

49:                                               ; preds = %.lr.ph, %.backedge
  %50 = phi i64 [ %41, %.lr.ph ], [ %115, %.backedge ]
  %51 = phi i64 [ %40, %.lr.ph ], [ %.be, %.backedge ]
  %52 = icmp sgt i64 %51, -1
  br i1 %52, label %55, label %53

53:                                               ; preds = %49
  %54 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

55:                                               ; preds = %49
  %56 = icmp slt i64 %51, %50
  br i1 %56, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %57

57:                                               ; preds = %55
  %58 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %55
  %59 = load ptr, ptr %2, align 8
  %60 = getelementptr i8, ptr %59, i64 %51
  %61 = load i8, ptr %60, align 1
  switch i8 %61, label %107 [
    i8 34, label %117
    i8 92, label %62
  ]

62:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %63 = add nuw nsw i64 %51, 1
  store i64 %63, ptr %3, align 8
  %64 = load i64, ptr %23, align 8
  %65 = add i64 %64, -2
  %66 = icmp eq i64 %51, %65
  br i1 %66, label %common.ret, label %67

67:                                               ; preds = %62
  %68 = icmp slt i64 %63, %64
  br i1 %68, label %rl_m_get__String_int64_t_r_int8_tRef.exit9, label %69

69:                                               ; preds = %67
  %70 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit9:       ; preds = %67
  %71 = load ptr, ptr %2, align 8
  %72 = getelementptr i8, ptr %71, i64 %63
  %73 = load i8, ptr %72, align 1
  %74 = icmp eq i8 %73, 34
  br i1 %74, label %75, label %107

75:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit9
  %76 = load i64, ptr %47, align 8
  %77 = icmp sgt i64 %76, 0
  br i1 %77, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i, label %78

78:                                               ; preds = %75
  %79 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i:     ; preds = %75
  %80 = load ptr, ptr %1, align 8
  %81 = getelementptr i8, ptr %80, i64 %76
  %82 = getelementptr i8, ptr %81, i64 -1
  store i8 34, ptr %82, align 1
  %83 = load i64, ptr %47, align 8
  %84 = add i64 %83, 1
  %85 = load i64, ptr %48, align 8
  %86 = icmp sgt i64 %85, %84
  br i1 %86, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i, label %87

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i
  %.pre2.i.i = load ptr, ptr %1, align 8
  br label %rl_m_append__String_int8_t.exit

87:                                               ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i
  %88 = shl i64 %84, 1
  %89 = tail call ptr @malloc(i64 %88)
  %90 = ptrtoint ptr %89 to i64
  %91 = icmp sgt i64 %88, 0
  br i1 %91, label %.lr.ph.preheader.i.i.i, label %.preheader2.i.i.i

.lr.ph.preheader.i.i.i:                           ; preds = %87
  tail call void @llvm.memset.p0.i64(ptr align 1 %89, i8 0, i64 %88, i1 false)
  br label %.preheader2.i.i.i

.preheader2.i.i.i:                                ; preds = %.lr.ph.preheader.i.i.i, %87
  %92 = icmp sgt i64 %83, 0
  %.pre.i.i.i = load ptr, ptr %1, align 8
  br i1 %92, label %.lr.ph5.i.i.i.preheader, label %.preheader.i.i.i

.lr.ph5.i.i.i.preheader:                          ; preds = %.preheader2.i.i.i
  %.pre.i.i.i77 = ptrtoint ptr %.pre.i.i.i to i64
  %min.iters.check = icmp ult i64 %83, 4
  %93 = sub i64 %90, %.pre.i.i.i77
  %diff.check = icmp ult i64 %93, 4
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph5.i.i.i.preheader78, label %vector.ph

.lr.ph5.i.i.i.preheader78:                        ; preds = %middle.block, %.lr.ph5.i.i.i.preheader
  %.14.i.i.i.ph = phi i64 [ 0, %.lr.ph5.i.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph5.i.i.i

vector.ph:                                        ; preds = %.lr.ph5.i.i.i.preheader
  %n.vec = and i64 %83, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %94 = getelementptr i8, ptr %89, i64 %index
  %95 = getelementptr i8, ptr %.pre.i.i.i, i64 %index
  %wide.load = load <4 x i8>, ptr %95, align 1
  store <4 x i8> %wide.load, ptr %94, align 1
  %index.next = add nuw i64 %index, 4
  %96 = icmp eq i64 %index.next, %n.vec
  br i1 %96, label %middle.block, label %vector.body, !llvm.loop !45

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %83, %n.vec
  br i1 %cmp.n, label %.preheader.i.i.i, label %.lr.ph5.i.i.i.preheader78

.preheader.i.i.i:                                 ; preds = %.lr.ph5.i.i.i, %middle.block, %.preheader2.i.i.i
  tail call void @free(ptr %.pre.i.i.i)
  store i64 %88, ptr %48, align 8
  store ptr %89, ptr %1, align 8
  %.pre.i.i = load i64, ptr %47, align 8
  br label %rl_m_append__String_int8_t.exit

.lr.ph5.i.i.i:                                    ; preds = %.lr.ph5.i.i.i.preheader78, %.lr.ph5.i.i.i
  %.14.i.i.i = phi i64 [ %100, %.lr.ph5.i.i.i ], [ %.14.i.i.i.ph, %.lr.ph5.i.i.i.preheader78 ]
  %97 = getelementptr i8, ptr %89, i64 %.14.i.i.i
  %98 = getelementptr i8, ptr %.pre.i.i.i, i64 %.14.i.i.i
  %99 = load i8, ptr %98, align 1
  store i8 %99, ptr %97, align 1
  %100 = add nuw nsw i64 %.14.i.i.i, 1
  %101 = icmp slt i64 %100, %83
  br i1 %101, label %.lr.ph5.i.i.i, label %.preheader.i.i.i, !llvm.loop !46

rl_m_append__String_int8_t.exit:                  ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i, %.preheader.i.i.i
  %102 = phi ptr [ %.pre2.i.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i ], [ %89, %.preheader.i.i.i ]
  %103 = phi i64 [ %83, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i ], [ %.pre.i.i, %.preheader.i.i.i ]
  %104 = getelementptr i8, ptr %102, i64 %103
  store i8 0, ptr %104, align 1
  %105 = load i64, ptr %47, align 8
  %106 = add i64 %105, 1
  store i64 %106, ptr %47, align 8
  br label %.backedge

107:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit9, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %108 = phi ptr [ %59, %rl_m_get__String_int64_t_r_int8_tRef.exit ], [ %71, %rl_m_get__String_int64_t_r_int8_tRef.exit9 ]
  %109 = phi i64 [ %50, %rl_m_get__String_int64_t_r_int8_tRef.exit ], [ %64, %rl_m_get__String_int64_t_r_int8_tRef.exit9 ]
  %110 = phi i64 [ %51, %rl_m_get__String_int64_t_r_int8_tRef.exit ], [ %63, %rl_m_get__String_int64_t_r_int8_tRef.exit9 ]
  %111 = icmp ult i64 %110, %109
  br i1 %111, label %rl_m_get__String_int64_t_r_int8_tRef.exit10, label %112

112:                                              ; preds = %107
  %113 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit10:      ; preds = %107
  %114 = getelementptr i8, ptr %108, i64 %110
  tail call void @rl_m_append__String_int8_t(ptr %1, ptr %114)
  br label %.backedge

.backedge:                                        ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit10, %rl_m_append__String_int8_t.exit
  %.be.in = load i64, ptr %3, align 8
  %.be = add i64 %.be.in, 1
  store i64 %.be, ptr %3, align 8
  %115 = load i64, ptr %23, align 8
  %116 = add i64 %115, -2
  %.not5 = icmp eq i64 %.be.in, %116
  br i1 %.not5, label %common.ret, label %49

117:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %118 = add nuw nsw i64 %51, 1
  store i64 %118, ptr %3, align 8
  br label %common.ret
}

; Function Attrs: nounwind
define void @rl_parse_string__bool_String_int64_t_r_bool(ptr nocapture writeonly %0, ptr nocapture writeonly %1, ptr nocapture readonly %2, ptr nocapture %3) local_unnamed_addr #5 {
  %.pr.i = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %.pr.i, -1
  br i1 %5, label %.lr.ph.i, label %._crit_edge.i

.lr.ph.i:                                         ; preds = %4
  %6 = getelementptr i8, ptr %2, i64 8
  %7 = load i64, ptr %6, align 8
  %8 = icmp slt i64 %.pr.i, %7
  br i1 %8, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i, label %._crit_edge

._crit_edge.i:                                    ; preds = %4
  %9 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

._crit_edge:                                      ; preds = %17, %.lr.ph.i
  %10 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit.i:      ; preds = %.lr.ph.i, %17
  %11 = phi i64 [ %19, %17 ], [ %7, %.lr.ph.i ]
  %12 = phi i64 [ %18, %17 ], [ %.pr.i, %.lr.ph.i ]
  %13 = load ptr, ptr %2, align 8
  %14 = getelementptr i8, ptr %13, i64 %12
  %15 = load i8, ptr %14, align 1
  switch i8 %15, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge [
    i8 32, label %rl_is_space__int8_t_r_bool.exit.thread.i
    i8 10, label %rl_is_space__int8_t_r_bool.exit.thread.i
    i8 0, label %rl_is_space__int8_t_r_bool.exit.thread.i
  ]

rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge: ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i
  %.pre = add nsw i64 %11, -1
  br label %rl__consume_space__String_int64_t.exit

rl_is_space__int8_t_r_bool.exit.thread.i:         ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i, %rl_m_get__String_int64_t_r_int8_tRef.exit.i, %rl_m_get__String_int64_t_r_int8_tRef.exit.i
  %16 = add nsw i64 %11, -1
  %.not8.i = icmp ult i64 %12, %16
  br i1 %.not8.i, label %17, label %rl__consume_space__String_int64_t.exit

17:                                               ; preds = %rl_is_space__int8_t_r_bool.exit.thread.i
  %18 = add nuw nsw i64 %12, 1
  store i64 %18, ptr %3, align 8
  %19 = load i64, ptr %6, align 8
  %20 = icmp slt i64 %18, %19
  br i1 %20, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i, label %._crit_edge

rl__consume_space__String_int64_t.exit:           ; preds = %rl_is_space__int8_t_r_bool.exit.thread.i, %rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge
  %.pre-phi = phi i64 [ %.pre, %rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge ], [ %16, %rl_is_space__int8_t_r_bool.exit.thread.i ]
  %.not.i = icmp slt i64 %12, %.pre-phi
  br i1 %.not.i, label %.lr.ph.preheader.i, label %common.ret

.lr.ph.preheader.i:                               ; preds = %rl__consume_space__String_int64_t.exit
  %21 = icmp sgt i64 %12, -1
  br i1 %21, label %.lr.ph.i4.preheader, label %35

.lr.ph.i4.preheader:                              ; preds = %.lr.ph.preheader.i
  %22 = icmp slt i64 %12, %11
  br i1 %22, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i5, label %37

.lr.ph.i4.1:                                      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i5
  %23 = add nuw nsw i64 %12, 1
  %24 = icmp ult i64 %23, %11
  br i1 %24, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.1, label %37

rl_m_get__String_int64_t_r_int8_tRef.exit.i5.1:   ; preds = %.lr.ph.i4.1
  %25 = getelementptr i8, ptr %13, i64 %23
  %26 = load i8, ptr %25, align 1
  %.not4.i.1 = icmp eq i8 %26, 114
  br i1 %.not4.i.1, label %.lr.ph.i4.2, label %common.ret

.lr.ph.i4.2:                                      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.1
  %27 = add nuw nsw i64 %12, 2
  %28 = icmp ult i64 %27, %11
  br i1 %28, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.2, label %37

rl_m_get__String_int64_t_r_int8_tRef.exit.i5.2:   ; preds = %.lr.ph.i4.2
  %29 = getelementptr i8, ptr %13, i64 %27
  %30 = load i8, ptr %29, align 1
  %.not4.i.2 = icmp eq i8 %30, 117
  br i1 %.not4.i.2, label %.lr.ph.i4.3, label %common.ret

.lr.ph.i4.3:                                      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.2
  %31 = add nuw nsw i64 %12, 3
  %32 = icmp ult i64 %31, %11
  br i1 %32, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.3, label %37

rl_m_get__String_int64_t_r_int8_tRef.exit.i5.3:   ; preds = %.lr.ph.i4.3
  %33 = getelementptr i8, ptr %13, i64 %31
  %34 = load i8, ptr %33, align 1
  %.not4.i.3 = icmp eq i8 %34, 101
  br i1 %.not4.i.3, label %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit, label %common.ret

35:                                               ; preds = %.lr.ph.preheader.i
  %36 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
  tail call void @llvm.trap()
  unreachable

37:                                               ; preds = %.lr.ph.i4.3, %.lr.ph.i4.2, %.lr.ph.i4.1, %.lr.ph.i4.preheader
  %38 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit.i5:     ; preds = %.lr.ph.i4.preheader
  switch i8 %15, label %common.ret [
    i8 116, label %.lr.ph.i4.1
    i8 102, label %.lr.ph.preheader.i10.1
  ]

.lr.ph.preheader.i10.1:                           ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i5
  %39 = add nuw nsw i64 %12, 1
  %40 = icmp ult i64 %39, %11
  br i1 %40, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.1, label %55

rl_m_get__String_int64_t_r_int8_tRef.exit.i13.1:  ; preds = %.lr.ph.preheader.i10.1
  %41 = getelementptr i8, ptr %13, i64 %39
  %42 = load i8, ptr %41, align 1
  %.not4.i14.1 = icmp eq i8 %42, 97
  br i1 %.not4.i14.1, label %.lr.ph.preheader.i10.2, label %common.ret

.lr.ph.preheader.i10.2:                           ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.1
  %43 = add nuw nsw i64 %12, 2
  %44 = icmp ult i64 %43, %11
  br i1 %44, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.2, label %55

rl_m_get__String_int64_t_r_int8_tRef.exit.i13.2:  ; preds = %.lr.ph.preheader.i10.2
  %45 = getelementptr i8, ptr %13, i64 %43
  %46 = load i8, ptr %45, align 1
  %.not4.i14.2 = icmp eq i8 %46, 108
  br i1 %.not4.i14.2, label %.lr.ph.preheader.i10.3, label %common.ret

.lr.ph.preheader.i10.3:                           ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.2
  %47 = add nuw nsw i64 %12, 3
  %48 = icmp ult i64 %47, %11
  br i1 %48, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.3, label %55

rl_m_get__String_int64_t_r_int8_tRef.exit.i13.3:  ; preds = %.lr.ph.preheader.i10.3
  %49 = getelementptr i8, ptr %13, i64 %47
  %50 = load i8, ptr %49, align 1
  %.not4.i14.3 = icmp eq i8 %50, 115
  br i1 %.not4.i14.3, label %.lr.ph.preheader.i10.4, label %common.ret

.lr.ph.preheader.i10.4:                           ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.3
  %51 = add nuw nsw i64 %12, 4
  %52 = icmp ult i64 %51, %11
  br i1 %52, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.4, label %55

rl_m_get__String_int64_t_r_int8_tRef.exit.i13.4:  ; preds = %.lr.ph.preheader.i10.4
  %53 = getelementptr i8, ptr %13, i64 %51
  %54 = load i8, ptr %53, align 1
  %.not4.i14.4 = icmp eq i8 %54, 101
  br i1 %.not4.i14.4, label %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit, label %common.ret

55:                                               ; preds = %.lr.ph.preheader.i10.4, %.lr.ph.preheader.i10.3, %.lr.ph.preheader.i10.2, %.lr.ph.preheader.i10.1
  %56 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

common.ret:                                       ; preds = %rl__consume_space__String_int64_t.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.4, %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.3, %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.2, %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.1, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.1, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.2, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.3, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5, %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit
  %.sink = phi i8 [ 1, %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.3 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.2 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.1 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.1 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.2 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.3 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.4 ], [ 0, %rl__consume_space__String_int64_t.exit ]
  store i8 %.sink, ptr %0, align 1
  ret void

rl_m_substring_matches__String_strlit_int64_t_r_bool.exit: ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.4, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.3
  %.sink45 = phi i8 [ 1, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.3 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.4 ]
  %.sink44 = phi i64 [ 4, %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.3 ], [ 5, %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.4 ]
  store i8 %.sink45, ptr %1, align 1
  %57 = load i64, ptr %3, align 8
  %58 = add i64 %57, %.sink44
  store i64 %58, ptr %3, align 8
  br label %common.ret
}

declare void @rl_print_string__String(ptr) local_unnamed_addr

declare void @rl_print_string_lit__strlit(ptr) local_unnamed_addr

define void @rl_print__String(ptr %0) local_unnamed_addr {
  tail call void @rl_print_string__String(ptr %0)
  ret void
}

define void @rl_print__strlit(ptr %0) local_unnamed_addr {
  tail call void @rl_print_string_lit__strlit(ptr %0)
  ret void
}

define i32 @main() local_unnamed_addr {
  %1 = alloca i64, align 8
  call void @rl_main__r_int64_t(ptr nonnull %1)
  %2 = load i64, ptr %1, align 8
  %3 = trunc i64 %2 to i32
  ret i32 %3
}

define void @rl_main__r_int64_t(ptr nocapture writeonly %0) local_unnamed_addr {
.lr.ph.i:
  %1 = alloca %String, align 8
  %2 = alloca %String, align 8
  %3 = alloca %String, align 8
  %4 = alloca %String, align 8
  %5 = alloca %String, align 8
  %6 = alloca %String, align 8
  %7 = alloca %String, align 8
  %8 = alloca %String, align 8
  %9 = alloca %String, align 8
  %10 = alloca %String, align 8
  %11 = alloca %String, align 8
  %12 = alloca %String, align 8
  %13 = alloca %String, align 8
  %14 = alloca %String, align 8
  %15 = alloca i64, align 8
  %16 = alloca %String, align 8
  %17 = alloca %String, align 8
  %18 = alloca ptr, align 8
  %19 = alloca %String, align 8
  %20 = alloca %String, align 8
  %21 = alloca %String, align 8
  %22 = alloca ptr, align 8
  %23 = alloca %String, align 8
  %24 = alloca %String, align 8
  %25 = alloca i64, align 8
  %26 = alloca %String, align 8
  %27 = alloca %String, align 8
  %28 = alloca ptr, align 8
  %29 = alloca %String, align 8
  %30 = alloca %String, align 8
  %31 = alloca %String, align 8
  %32 = alloca ptr, align 8
  %33 = alloca i8, align 1
  %34 = alloca %Dict, align 8
  %35 = alloca %Vector.1, align 8
  %36 = alloca %Vector.1, align 8
  %37 = alloca ptr, align 8
  %38 = alloca i8, align 1
  %39 = alloca %String, align 8
  %40 = alloca %String, align 8
  %41 = alloca i64, align 8
  %42 = alloca %String, align 8
  %43 = alloca %String, align 8
  %44 = alloca ptr, align 8
  %45 = alloca %String, align 8
  %46 = alloca %String, align 8
  %47 = alloca %String, align 8
  %48 = alloca ptr, align 8
  %49 = alloca i64, align 8
  %50 = alloca i8, align 1
  %51 = alloca i64, align 8
  %52 = alloca i64, align 8
  %53 = alloca %Dict, align 8
  %54 = getelementptr inbounds i8, ptr %53, i64 16
  store i64 4, ptr %54, align 8
  %55 = getelementptr inbounds i8, ptr %53, i64 8
  store i64 0, ptr %55, align 8
  %56 = getelementptr inbounds i8, ptr %53, i64 24
  store double 7.500000e-01, ptr %56, align 8
  %57 = tail call dereferenceable_or_null(128) ptr @malloc(i64 128)
  store ptr %57, ptr %53, align 8
  store i8 0, ptr %57, align 1
  %58 = getelementptr i8, ptr %57, i64 32
  store i8 0, ptr %58, align 1
  %59 = getelementptr i8, ptr %57, i64 64
  store i8 0, ptr %59, align 1
  %60 = getelementptr i8, ptr %57, i64 96
  store i8 0, ptr %60, align 1
  br label %rl_m_init__DictTint64_tTint64_tT.exit

.preheader128:                                    ; preds = %rl_m_init__DictTint64_tTint64_tT.exit
  %61 = getelementptr inbounds i8, ptr %12, i64 8
  %62 = getelementptr inbounds i8, ptr %12, i64 16
  %63 = getelementptr inbounds i8, ptr %11, i64 8
  %64 = getelementptr inbounds i8, ptr %11, i64 16
  %65 = getelementptr inbounds i8, ptr %10, i64 8
  %66 = getelementptr inbounds i8, ptr %10, i64 16
  %67 = getelementptr inbounds i8, ptr %9, i64 8
  %68 = getelementptr inbounds i8, ptr %9, i64 16
  %69 = getelementptr inbounds i8, ptr %47, i64 16
  %70 = getelementptr inbounds i8, ptr %47, i64 8
  %71 = getelementptr inbounds i8, ptr %46, i64 16
  %72 = getelementptr inbounds i8, ptr %46, i64 8
  %73 = getelementptr inbounds i8, ptr %45, i64 16
  %74 = getelementptr inbounds i8, ptr %45, i64 8
  %75 = getelementptr inbounds i8, ptr %43, i64 16
  %76 = getelementptr inbounds i8, ptr %43, i64 8
  %77 = getelementptr inbounds i8, ptr %42, i64 16
  %78 = getelementptr inbounds i8, ptr %42, i64 8
  %79 = getelementptr inbounds i8, ptr %40, i64 16
  %80 = getelementptr inbounds i8, ptr %40, i64 8
  %81 = getelementptr inbounds i8, ptr %39, i64 16
  %82 = getelementptr inbounds i8, ptr %39, i64 8
  %.pre = load i64, ptr %55, align 8
  br label %85

rl_m_init__DictTint64_tTint64_tT.exit:            ; preds = %.lr.ph.i, %rl_m_init__DictTint64_tTint64_tT.exit
  %.0119131 = phi i64 [ %83, %rl_m_init__DictTint64_tTint64_tT.exit ], [ 0, %.lr.ph.i ]
  store i64 %.0119131, ptr %52, align 8
  %83 = add nuw nsw i64 %.0119131, 1
  %84 = mul i64 %.0119131, %.0119131
  store i64 %84, ptr %51, align 8
  call void @rl_m_insert__DictTint64_tTint64_tT_int64_t_int64_t_r_bool(ptr nonnull %50, ptr nonnull %53, ptr nonnull %52, ptr nonnull %51)
  %.not = icmp eq i64 %83, 50
  br i1 %.not, label %.preheader128, label %rl_m_init__DictTint64_tTint64_tT.exit

85:                                               ; preds = %.preheader128, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread
  %86 = phi i64 [ %.pre, %.preheader128 ], [ %186, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread ]
  %.0121132 = phi i64 [ 0, %.preheader128 ], [ %87, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread ]
  %87 = add nuw nsw i64 %.0121132, 1
  %88 = mul nuw nsw i64 %.0121132, 10
  store i64 %88, ptr %49, align 8
  %89 = icmp eq i64 %86, 0
  br i1 %89, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %90

90:                                               ; preds = %85
  %91 = lshr i64 %88, 33
  %92 = xor i64 %91, %88
  %93 = mul i64 %92, 1099511628211
  %94 = lshr i64 %93, 33
  %95 = xor i64 %94, %93
  %96 = mul i64 %95, 16777619
  %97 = lshr i64 %96, 33
  %.masked.i.i.i.i = and i64 %96, 9223372036854775807
  %98 = xor i64 %.masked.i.i.i.i, %97
  %99 = load i64, ptr %54, align 8
  %.not13.i = icmp sgt i64 %99, 0
  br i1 %.not13.i, label %.lr.ph.i8, label %._crit_edge.i

.lr.ph.i8:                                        ; preds = %90
  %100 = load ptr, ptr %53, align 8
  br label %102

._crit_edge.i:                                    ; preds = %90, %119
  %101 = call i32 @puts(ptr nonnull dereferenceable(1) @str_1)
  call void @llvm.trap()
  unreachable

102:                                              ; preds = %119, %.lr.ph.i8
  %.pn.i = phi i64 [ %98, %.lr.ph.i8 ], [ %120, %119 ]
  %.0215.i = phi i64 [ 0, %.lr.ph.i8 ], [ %103, %119 ]
  %.0416.i = srem i64 %.pn.i, %99
  %103 = add nuw nsw i64 %.0215.i, 1
  %104 = getelementptr %Entry, ptr %100, i64 %.0416.i
  %105 = load i8, ptr %104, align 1
  %106 = icmp eq i8 %105, 0
  br i1 %106, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %107

107:                                              ; preds = %102
  %108 = getelementptr i8, ptr %104, i64 8
  %109 = load i64, ptr %108, align 8
  %110 = icmp eq i64 %109, %98
  br i1 %110, label %111, label %.thread.i

111:                                              ; preds = %107
  %112 = getelementptr i8, ptr %104, i64 16
  %113 = load i64, ptr %112, align 8
  %.not11.i = icmp eq i64 %113, %88
  br i1 %.not11.i, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit, label %.thread.i

.thread.i:                                        ; preds = %111, %107
  %114 = add i64 %.0416.i, %99
  %115 = srem i64 %109, %99
  %116 = sub i64 %114, %115
  %117 = srem i64 %116, %99
  %118 = icmp slt i64 %117, %.0215.i
  br i1 %118, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %119

119:                                              ; preds = %.thread.i
  %120 = add i64 %.0416.i, 1
  %.not.i = icmp slt i64 %103, %99
  br i1 %.not.i, label %102, label %._crit_edge.i

rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit: ; preds = %111
  store ptr @str_16, ptr %48, align 8
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %11)
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %12)
  store i64 4, ptr %62, align 8
  %121 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %121, ptr %12, align 8
  store i32 0, ptr %121, align 1
  store i64 1, ptr %61, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %12, ptr nonnull readonly %48)
  store i64 4, ptr %64, align 8
  %122 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %122, ptr %11, align 8
  store i32 0, ptr %122, align 1
  store i64 1, ptr %63, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %11, ptr nonnull readonly %12)
  %123 = load i64, ptr %62, align 8
  %.not2.i.i.i = icmp eq i64 %123, 0
  br i1 %.not2.i.i.i, label %rl_s__strlit_r_String.exit, label %124

124:                                              ; preds = %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit
  %125 = load ptr, ptr %12, align 8
  call void @free(ptr %125)
  br label %rl_s__strlit_r_String.exit

rl_s__strlit_r_String.exit:                       ; preds = %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit, %124
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %47, ptr noundef nonnull align 8 dereferenceable(24) %11, i64 24, i1 false)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %11)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %12)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %46, ptr nonnull %49)
  call void @rl_m_add__String_String_r_String(ptr nonnull %45, ptr nonnull %47, ptr nonnull %46)
  store ptr @str_17, ptr %44, align 8
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %9)
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %10)
  store i64 4, ptr %66, align 8
  %126 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %126, ptr %10, align 8
  store i32 0, ptr %126, align 1
  store i64 1, ptr %65, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %10, ptr nonnull readonly %44)
  store i64 4, ptr %68, align 8
  %127 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %127, ptr %9, align 8
  store i32 0, ptr %127, align 1
  store i64 1, ptr %67, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %9, ptr nonnull readonly %10)
  %128 = load i64, ptr %66, align 8
  %.not2.i.i.i9 = icmp eq i64 %128, 0
  br i1 %.not2.i.i.i9, label %rl_s__strlit_r_String.exit10, label %129

129:                                              ; preds = %rl_s__strlit_r_String.exit
  %130 = load ptr, ptr %10, align 8
  call void @free(ptr %130)
  br label %rl_s__strlit_r_String.exit10

rl_s__strlit_r_String.exit10:                     ; preds = %rl_s__strlit_r_String.exit, %129
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %43, ptr noundef nonnull align 8 dereferenceable(24) %9, i64 24, i1 false)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %9)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %10)
  call void @rl_m_add__String_String_r_String(ptr nonnull %42, ptr nonnull %45, ptr nonnull %43)
  call void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr nonnull %41, ptr nonnull %53, ptr nonnull %49)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %40, ptr nonnull %41)
  call void @rl_m_add__String_String_r_String(ptr nonnull %39, ptr nonnull %42, ptr nonnull %40)
  call void @rl_print_string__String(ptr nonnull %39)
  %131 = load i64, ptr %69, align 8
  %.not2.i.i = icmp eq i64 %131, 0
  br i1 %.not2.i.i, label %rl_m_drop__String.exit, label %132

132:                                              ; preds = %rl_s__strlit_r_String.exit10
  %133 = load ptr, ptr %47, align 8
  call void @free(ptr %133)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %rl_s__strlit_r_String.exit10, %132
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %70, i8 0, i64 16, i1 false)
  %134 = load i64, ptr %71, align 8
  %.not2.i.i11 = icmp eq i64 %134, 0
  br i1 %.not2.i.i11, label %rl_m_drop__String.exit12, label %135

135:                                              ; preds = %rl_m_drop__String.exit
  %136 = load ptr, ptr %46, align 8
  call void @free(ptr %136)
  br label %rl_m_drop__String.exit12

rl_m_drop__String.exit12:                         ; preds = %rl_m_drop__String.exit, %135
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %72, i8 0, i64 16, i1 false)
  %137 = load i64, ptr %73, align 8
  %.not2.i.i13 = icmp eq i64 %137, 0
  br i1 %.not2.i.i13, label %rl_m_drop__String.exit14, label %138

138:                                              ; preds = %rl_m_drop__String.exit12
  %139 = load ptr, ptr %45, align 8
  call void @free(ptr %139)
  br label %rl_m_drop__String.exit14

rl_m_drop__String.exit14:                         ; preds = %rl_m_drop__String.exit12, %138
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %74, i8 0, i64 16, i1 false)
  %140 = load i64, ptr %75, align 8
  %.not2.i.i15 = icmp eq i64 %140, 0
  br i1 %.not2.i.i15, label %rl_m_drop__String.exit16, label %141

141:                                              ; preds = %rl_m_drop__String.exit14
  %142 = load ptr, ptr %43, align 8
  call void @free(ptr %142)
  br label %rl_m_drop__String.exit16

rl_m_drop__String.exit16:                         ; preds = %rl_m_drop__String.exit14, %141
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %76, i8 0, i64 16, i1 false)
  %143 = load i64, ptr %77, align 8
  %.not2.i.i17 = icmp eq i64 %143, 0
  br i1 %.not2.i.i17, label %rl_m_drop__String.exit18, label %144

144:                                              ; preds = %rl_m_drop__String.exit16
  %145 = load ptr, ptr %42, align 8
  call void @free(ptr %145)
  br label %rl_m_drop__String.exit18

rl_m_drop__String.exit18:                         ; preds = %rl_m_drop__String.exit16, %144
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %78, i8 0, i64 16, i1 false)
  %146 = load i64, ptr %79, align 8
  %.not2.i.i19 = icmp eq i64 %146, 0
  br i1 %.not2.i.i19, label %rl_m_drop__String.exit20, label %147

147:                                              ; preds = %rl_m_drop__String.exit18
  %148 = load ptr, ptr %40, align 8
  call void @free(ptr %148)
  br label %rl_m_drop__String.exit20

rl_m_drop__String.exit20:                         ; preds = %rl_m_drop__String.exit18, %147
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %80, i8 0, i64 16, i1 false)
  %149 = load i64, ptr %81, align 8
  %.not2.i.i21 = icmp eq i64 %149, 0
  br i1 %.not2.i.i21, label %rl_m_drop__String.exit22, label %150

150:                                              ; preds = %rl_m_drop__String.exit20
  %151 = load ptr, ptr %39, align 8
  call void @free(ptr %151)
  br label %rl_m_drop__String.exit22

rl_m_drop__String.exit22:                         ; preds = %rl_m_drop__String.exit20, %150
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %82, i8 0, i64 16, i1 false)
  call void @rl_m_remove__DictTint64_tTint64_tT_int64_t_r_bool(ptr nonnull %38, ptr nonnull %53, ptr nonnull %49)
  %152 = load i64, ptr %55, align 8
  %153 = icmp eq i64 %152, 0
  br i1 %153, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %154

154:                                              ; preds = %rl_m_drop__String.exit22
  %155 = load i64, ptr %49, align 8
  %156 = lshr i64 %155, 33
  %157 = xor i64 %156, %155
  %158 = mul i64 %157, 1099511628211
  %159 = lshr i64 %158, 33
  %160 = xor i64 %159, %158
  %161 = mul i64 %160, 16777619
  %162 = lshr i64 %161, 33
  %.masked.i.i.i.i23 = and i64 %161, 9223372036854775807
  %163 = xor i64 %.masked.i.i.i.i23, %162
  %164 = load i64, ptr %54, align 8
  %.not13.i24 = icmp sgt i64 %164, 0
  br i1 %.not13.i24, label %.lr.ph.i26, label %._crit_edge.i25

.lr.ph.i26:                                       ; preds = %154
  %165 = load ptr, ptr %53, align 8
  br label %167

._crit_edge.i25:                                  ; preds = %154, %184
  %166 = call i32 @puts(ptr nonnull dereferenceable(1) @str_1)
  call void @llvm.trap()
  unreachable

167:                                              ; preds = %184, %.lr.ph.i26
  %.pn.i27 = phi i64 [ %163, %.lr.ph.i26 ], [ %185, %184 ]
  %.0215.i28 = phi i64 [ 0, %.lr.ph.i26 ], [ %168, %184 ]
  %.0416.i29 = srem i64 %.pn.i27, %164
  %168 = add nuw nsw i64 %.0215.i28, 1
  %169 = getelementptr %Entry, ptr %165, i64 %.0416.i29
  %170 = load i8, ptr %169, align 1
  %171 = icmp eq i8 %170, 0
  br i1 %171, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %172

172:                                              ; preds = %167
  %173 = getelementptr i8, ptr %169, i64 8
  %174 = load i64, ptr %173, align 8
  %175 = icmp eq i64 %174, %163
  br i1 %175, label %176, label %.thread.i30

176:                                              ; preds = %172
  %177 = getelementptr i8, ptr %169, i64 16
  %178 = load i64, ptr %177, align 8
  %.not11.i33 = icmp eq i64 %178, %155
  br i1 %.not11.i33, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34, label %.thread.i30

.thread.i30:                                      ; preds = %176, %172
  %179 = add i64 %.0416.i29, %164
  %180 = srem i64 %174, %164
  %181 = sub i64 %179, %180
  %182 = srem i64 %181, %164
  %183 = icmp slt i64 %182, %.0215.i28
  br i1 %183, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %184

184:                                              ; preds = %.thread.i30
  %185 = add i64 %.0416.i29, 1
  %.not.i31 = icmp slt i64 %168, %164
  br i1 %.not.i31, label %167, label %._crit_edge.i25

rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34: ; preds = %176
  store ptr @str_18, ptr %37, align 8
  call void @rl_print_string_lit__strlit(ptr nonnull %37)
  br label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread

rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread: ; preds = %.thread.i, %102, %.thread.i30, %167, %rl_m_drop__String.exit22, %85, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34
  %186 = phi i64 [ 0, %rl_m_drop__String.exit22 ], [ 0, %85 ], [ %152, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34 ], [ %152, %167 ], [ %152, %.thread.i30 ], [ %86, %102 ], [ %86, %.thread.i ]
  %.not2 = icmp eq i64 %87, 5
  br i1 %.not2, label %.lr.ph.i35, label %85

.lr.ph.i35:                                       ; preds = %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread
  call void @rl_m_keys__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr nonnull %36, ptr nonnull %53)
  call void @rl_m_values__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr nonnull %35, ptr nonnull %53)
  %187 = getelementptr inbounds i8, ptr %34, i64 16
  store i64 4, ptr %187, align 8
  %188 = getelementptr inbounds i8, ptr %34, i64 8
  store i64 0, ptr %188, align 8
  %189 = getelementptr inbounds i8, ptr %34, i64 24
  store double 7.500000e-01, ptr %189, align 8
  %190 = call dereferenceable_or_null(128) ptr @malloc(i64 128)
  store ptr %190, ptr %34, align 8
  store i8 0, ptr %190, align 1
  %191 = getelementptr i8, ptr %190, i64 32
  store i8 0, ptr %191, align 1
  %192 = getelementptr i8, ptr %190, i64 64
  store i8 0, ptr %192, align 1
  %193 = getelementptr i8, ptr %190, i64 96
  store i8 0, ptr %193, align 1
  %194 = getelementptr inbounds i8, ptr %36, i64 8
  %195 = load i64, ptr %194, align 8
  %.not3133 = icmp eq i64 %195, 0
  br i1 %.not3133, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.i35
  %196 = getelementptr inbounds i8, ptr %35, i64 8
  %197 = load i64, ptr %196, align 8
  %198 = load ptr, ptr %35, align 8
  %199 = load ptr, ptr %36, align 8
  br label %222

.lr.ph137:                                        ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39
  %200 = getelementptr inbounds i8, ptr %8, i64 8
  %201 = getelementptr inbounds i8, ptr %8, i64 16
  %202 = getelementptr inbounds i8, ptr %7, i64 8
  %203 = getelementptr inbounds i8, ptr %7, i64 16
  %204 = getelementptr inbounds i8, ptr %6, i64 8
  %205 = getelementptr inbounds i8, ptr %6, i64 16
  %206 = getelementptr inbounds i8, ptr %5, i64 8
  %207 = getelementptr inbounds i8, ptr %5, i64 16
  %208 = getelementptr inbounds i8, ptr %31, i64 16
  %209 = getelementptr inbounds i8, ptr %31, i64 8
  %210 = getelementptr inbounds i8, ptr %30, i64 16
  %211 = getelementptr inbounds i8, ptr %30, i64 8
  %212 = getelementptr inbounds i8, ptr %29, i64 16
  %213 = getelementptr inbounds i8, ptr %29, i64 8
  %214 = getelementptr inbounds i8, ptr %27, i64 16
  %215 = getelementptr inbounds i8, ptr %27, i64 8
  %216 = getelementptr inbounds i8, ptr %26, i64 16
  %217 = getelementptr inbounds i8, ptr %26, i64 8
  %218 = getelementptr inbounds i8, ptr %24, i64 16
  %219 = getelementptr inbounds i8, ptr %24, i64 8
  %220 = getelementptr inbounds i8, ptr %23, i64 16
  %221 = getelementptr inbounds i8, ptr %23, i64 8
  br label %233

222:                                              ; preds = %.lr.ph, %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39
  %.0120134 = phi i64 [ 0, %.lr.ph ], [ %223, %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39 ]
  %223 = add nuw i64 %.0120134, 1
  %224 = icmp slt i64 %.0120134, %197
  br i1 %224, label %227, label %225

225:                                              ; preds = %222
  %226 = call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  call void @llvm.trap()
  unreachable

227:                                              ; preds = %222
  %228 = icmp slt i64 %.0120134, %195
  br i1 %228, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39, label %229

229:                                              ; preds = %227
  %230 = call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  call void @llvm.trap()
  unreachable

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39: ; preds = %227
  %231 = getelementptr i64, ptr %198, i64 %.0120134
  %232 = getelementptr i64, ptr %199, i64 %.0120134
  call void @rl_m_insert__DictTint64_tTint64_tT_int64_t_int64_t_r_bool(ptr nonnull %33, ptr nonnull %34, ptr %231, ptr %232)
  %.not3 = icmp eq i64 %223, %195
  br i1 %.not3, label %.lr.ph137, label %222

233:                                              ; preds = %.lr.ph137, %rl_m_drop__String.exit58
  %.0118136 = phi i64 [ 0, %.lr.ph137 ], [ %239, %rl_m_drop__String.exit58 ]
  %234 = icmp slt i64 %.0118136, %195
  br i1 %234, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit40, label %235

235:                                              ; preds = %233
  %236 = call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  call void @llvm.trap()
  unreachable

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit40: ; preds = %233
  %237 = load ptr, ptr %36, align 8
  %238 = getelementptr i64, ptr %237, i64 %.0118136
  %239 = add nuw nsw i64 %.0118136, 1
  store ptr @str_19, ptr %32, align 8
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %7)
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %8)
  store i64 4, ptr %201, align 8
  %240 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %240, ptr %8, align 8
  store i32 0, ptr %240, align 1
  store i64 1, ptr %200, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %8, ptr nonnull readonly %32)
  store i64 4, ptr %203, align 8
  %241 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %241, ptr %7, align 8
  store i32 0, ptr %241, align 1
  store i64 1, ptr %202, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %7, ptr nonnull readonly %8)
  %242 = load i64, ptr %201, align 8
  %.not2.i.i.i41 = icmp eq i64 %242, 0
  br i1 %.not2.i.i.i41, label %rl_s__strlit_r_String.exit42, label %243

243:                                              ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit40
  %244 = load ptr, ptr %8, align 8
  call void @free(ptr %244)
  br label %rl_s__strlit_r_String.exit42

rl_s__strlit_r_String.exit42:                     ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit40, %243
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %31, ptr noundef nonnull align 8 dereferenceable(24) %7, i64 24, i1 false)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %7)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %8)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %30, ptr %238)
  call void @rl_m_add__String_String_r_String(ptr nonnull %29, ptr nonnull %31, ptr nonnull %30)
  store ptr @str_20, ptr %28, align 8
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %5)
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %6)
  store i64 4, ptr %205, align 8
  %245 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %245, ptr %6, align 8
  store i32 0, ptr %245, align 1
  store i64 1, ptr %204, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %6, ptr nonnull readonly %28)
  store i64 4, ptr %207, align 8
  %246 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %246, ptr %5, align 8
  store i32 0, ptr %246, align 1
  store i64 1, ptr %206, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %5, ptr nonnull readonly %6)
  %247 = load i64, ptr %205, align 8
  %.not2.i.i.i43 = icmp eq i64 %247, 0
  br i1 %.not2.i.i.i43, label %rl_s__strlit_r_String.exit44, label %248

248:                                              ; preds = %rl_s__strlit_r_String.exit42
  %249 = load ptr, ptr %6, align 8
  call void @free(ptr %249)
  br label %rl_s__strlit_r_String.exit44

rl_s__strlit_r_String.exit44:                     ; preds = %rl_s__strlit_r_String.exit42, %248
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %27, ptr noundef nonnull align 8 dereferenceable(24) %5, i64 24, i1 false)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %5)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %6)
  call void @rl_m_add__String_String_r_String(ptr nonnull %26, ptr nonnull %29, ptr nonnull %27)
  call void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr nonnull %25, ptr nonnull %53, ptr %238)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %24, ptr nonnull %25)
  call void @rl_m_add__String_String_r_String(ptr nonnull %23, ptr nonnull %26, ptr nonnull %24)
  call void @rl_print_string__String(ptr nonnull %23)
  %250 = load i64, ptr %208, align 8
  %.not2.i.i45 = icmp eq i64 %250, 0
  br i1 %.not2.i.i45, label %rl_m_drop__String.exit46, label %251

251:                                              ; preds = %rl_s__strlit_r_String.exit44
  %252 = load ptr, ptr %31, align 8
  call void @free(ptr %252)
  br label %rl_m_drop__String.exit46

rl_m_drop__String.exit46:                         ; preds = %rl_s__strlit_r_String.exit44, %251
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %209, i8 0, i64 16, i1 false)
  %253 = load i64, ptr %210, align 8
  %.not2.i.i47 = icmp eq i64 %253, 0
  br i1 %.not2.i.i47, label %rl_m_drop__String.exit48, label %254

254:                                              ; preds = %rl_m_drop__String.exit46
  %255 = load ptr, ptr %30, align 8
  call void @free(ptr %255)
  br label %rl_m_drop__String.exit48

rl_m_drop__String.exit48:                         ; preds = %rl_m_drop__String.exit46, %254
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %211, i8 0, i64 16, i1 false)
  %256 = load i64, ptr %212, align 8
  %.not2.i.i49 = icmp eq i64 %256, 0
  br i1 %.not2.i.i49, label %rl_m_drop__String.exit50, label %257

257:                                              ; preds = %rl_m_drop__String.exit48
  %258 = load ptr, ptr %29, align 8
  call void @free(ptr %258)
  br label %rl_m_drop__String.exit50

rl_m_drop__String.exit50:                         ; preds = %rl_m_drop__String.exit48, %257
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %213, i8 0, i64 16, i1 false)
  %259 = load i64, ptr %214, align 8
  %.not2.i.i51 = icmp eq i64 %259, 0
  br i1 %.not2.i.i51, label %rl_m_drop__String.exit52, label %260

260:                                              ; preds = %rl_m_drop__String.exit50
  %261 = load ptr, ptr %27, align 8
  call void @free(ptr %261)
  br label %rl_m_drop__String.exit52

rl_m_drop__String.exit52:                         ; preds = %rl_m_drop__String.exit50, %260
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %215, i8 0, i64 16, i1 false)
  %262 = load i64, ptr %216, align 8
  %.not2.i.i53 = icmp eq i64 %262, 0
  br i1 %.not2.i.i53, label %rl_m_drop__String.exit54, label %263

263:                                              ; preds = %rl_m_drop__String.exit52
  %264 = load ptr, ptr %26, align 8
  call void @free(ptr %264)
  br label %rl_m_drop__String.exit54

rl_m_drop__String.exit54:                         ; preds = %rl_m_drop__String.exit52, %263
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %217, i8 0, i64 16, i1 false)
  %265 = load i64, ptr %218, align 8
  %.not2.i.i55 = icmp eq i64 %265, 0
  br i1 %.not2.i.i55, label %rl_m_drop__String.exit56, label %266

266:                                              ; preds = %rl_m_drop__String.exit54
  %267 = load ptr, ptr %24, align 8
  call void @free(ptr %267)
  br label %rl_m_drop__String.exit56

rl_m_drop__String.exit56:                         ; preds = %rl_m_drop__String.exit54, %266
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %219, i8 0, i64 16, i1 false)
  %268 = load i64, ptr %220, align 8
  %.not2.i.i57 = icmp eq i64 %268, 0
  br i1 %.not2.i.i57, label %rl_m_drop__String.exit58, label %269

269:                                              ; preds = %rl_m_drop__String.exit56
  %270 = load ptr, ptr %23, align 8
  call void @free(ptr %270)
  br label %rl_m_drop__String.exit58

rl_m_drop__String.exit58:                         ; preds = %rl_m_drop__String.exit56, %269
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %221, i8 0, i64 16, i1 false)
  %.not4 = icmp eq i64 %239, %195
  br i1 %.not4, label %._crit_edge, label %233

._crit_edge:                                      ; preds = %rl_m_drop__String.exit58, %.lr.ph.i35
  %271 = getelementptr inbounds i8, ptr %35, i64 8
  %272 = load i64, ptr %271, align 8
  %.not5138 = icmp eq i64 %272, 0
  br i1 %.not5138, label %._crit_edge142, label %.lr.ph141

.lr.ph141:                                        ; preds = %._crit_edge
  %273 = getelementptr inbounds i8, ptr %4, i64 8
  %274 = getelementptr inbounds i8, ptr %4, i64 16
  %275 = getelementptr inbounds i8, ptr %3, i64 8
  %276 = getelementptr inbounds i8, ptr %3, i64 16
  %277 = getelementptr inbounds i8, ptr %2, i64 8
  %278 = getelementptr inbounds i8, ptr %2, i64 16
  %279 = getelementptr inbounds i8, ptr %1, i64 8
  %280 = getelementptr inbounds i8, ptr %1, i64 16
  %281 = getelementptr inbounds i8, ptr %21, i64 16
  %282 = getelementptr inbounds i8, ptr %21, i64 8
  %283 = getelementptr inbounds i8, ptr %20, i64 16
  %284 = getelementptr inbounds i8, ptr %20, i64 8
  %285 = getelementptr inbounds i8, ptr %19, i64 16
  %286 = getelementptr inbounds i8, ptr %19, i64 8
  %287 = getelementptr inbounds i8, ptr %17, i64 16
  %288 = getelementptr inbounds i8, ptr %17, i64 8
  %289 = getelementptr inbounds i8, ptr %16, i64 16
  %290 = getelementptr inbounds i8, ptr %16, i64 8
  %291 = getelementptr inbounds i8, ptr %14, i64 16
  %292 = getelementptr inbounds i8, ptr %14, i64 8
  %293 = getelementptr inbounds i8, ptr %13, i64 16
  %294 = getelementptr inbounds i8, ptr %13, i64 8
  br label %295

295:                                              ; preds = %.lr.ph141, %rl_m_drop__String.exit77
  %.0139 = phi i64 [ 0, %.lr.ph141 ], [ %301, %rl_m_drop__String.exit77 ]
  %296 = icmp slt i64 %.0139, %272
  br i1 %296, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit59, label %297

297:                                              ; preds = %295
  %298 = call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  call void @llvm.trap()
  unreachable

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit59: ; preds = %295
  %299 = load ptr, ptr %35, align 8
  %300 = getelementptr i64, ptr %299, i64 %.0139
  %301 = add nuw nsw i64 %.0139, 1
  store ptr @str_21, ptr %22, align 8
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %3)
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %4)
  store i64 4, ptr %274, align 8
  %302 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %302, ptr %4, align 8
  store i32 0, ptr %302, align 1
  store i64 1, ptr %273, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %4, ptr nonnull readonly %22)
  store i64 4, ptr %276, align 8
  %303 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %303, ptr %3, align 8
  store i32 0, ptr %303, align 1
  store i64 1, ptr %275, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %3, ptr nonnull readonly %4)
  %304 = load i64, ptr %274, align 8
  %.not2.i.i.i60 = icmp eq i64 %304, 0
  br i1 %.not2.i.i.i60, label %rl_s__strlit_r_String.exit61, label %305

305:                                              ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit59
  %306 = load ptr, ptr %4, align 8
  call void @free(ptr %306)
  br label %rl_s__strlit_r_String.exit61

rl_s__strlit_r_String.exit61:                     ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit59, %305
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %21, ptr noundef nonnull align 8 dereferenceable(24) %3, i64 24, i1 false)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %3)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %4)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %20, ptr %300)
  call void @rl_m_add__String_String_r_String(ptr nonnull %19, ptr nonnull %21, ptr nonnull %20)
  store ptr @str_20, ptr %18, align 8
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %1)
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %2)
  store i64 4, ptr %278, align 8
  %307 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %307, ptr %2, align 8
  store i32 0, ptr %307, align 1
  store i64 1, ptr %277, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %2, ptr nonnull readonly %18)
  store i64 4, ptr %280, align 8
  %308 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %308, ptr %1, align 8
  store i32 0, ptr %308, align 1
  store i64 1, ptr %279, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %1, ptr nonnull readonly %2)
  %309 = load i64, ptr %278, align 8
  %.not2.i.i.i62 = icmp eq i64 %309, 0
  br i1 %.not2.i.i.i62, label %rl_s__strlit_r_String.exit63, label %310

310:                                              ; preds = %rl_s__strlit_r_String.exit61
  %311 = load ptr, ptr %2, align 8
  call void @free(ptr %311)
  br label %rl_s__strlit_r_String.exit63

rl_s__strlit_r_String.exit63:                     ; preds = %rl_s__strlit_r_String.exit61, %310
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %17, ptr noundef nonnull align 8 dereferenceable(24) %1, i64 24, i1 false)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %1)
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %2)
  call void @rl_m_add__String_String_r_String(ptr nonnull %16, ptr nonnull %19, ptr nonnull %17)
  call void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr nonnull %15, ptr nonnull %34, ptr %300)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %14, ptr nonnull %15)
  call void @rl_m_add__String_String_r_String(ptr nonnull %13, ptr nonnull %16, ptr nonnull %14)
  call void @rl_print_string__String(ptr nonnull %13)
  %312 = load i64, ptr %281, align 8
  %.not2.i.i64 = icmp eq i64 %312, 0
  br i1 %.not2.i.i64, label %rl_m_drop__String.exit65, label %313

313:                                              ; preds = %rl_s__strlit_r_String.exit63
  %314 = load ptr, ptr %21, align 8
  call void @free(ptr %314)
  br label %rl_m_drop__String.exit65

rl_m_drop__String.exit65:                         ; preds = %rl_s__strlit_r_String.exit63, %313
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %282, i8 0, i64 16, i1 false)
  %315 = load i64, ptr %283, align 8
  %.not2.i.i66 = icmp eq i64 %315, 0
  br i1 %.not2.i.i66, label %rl_m_drop__String.exit67, label %316

316:                                              ; preds = %rl_m_drop__String.exit65
  %317 = load ptr, ptr %20, align 8
  call void @free(ptr %317)
  br label %rl_m_drop__String.exit67

rl_m_drop__String.exit67:                         ; preds = %rl_m_drop__String.exit65, %316
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %284, i8 0, i64 16, i1 false)
  %318 = load i64, ptr %285, align 8
  %.not2.i.i68 = icmp eq i64 %318, 0
  br i1 %.not2.i.i68, label %rl_m_drop__String.exit69, label %319

319:                                              ; preds = %rl_m_drop__String.exit67
  %320 = load ptr, ptr %19, align 8
  call void @free(ptr %320)
  br label %rl_m_drop__String.exit69

rl_m_drop__String.exit69:                         ; preds = %rl_m_drop__String.exit67, %319
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %286, i8 0, i64 16, i1 false)
  %321 = load i64, ptr %287, align 8
  %.not2.i.i70 = icmp eq i64 %321, 0
  br i1 %.not2.i.i70, label %rl_m_drop__String.exit71, label %322

322:                                              ; preds = %rl_m_drop__String.exit69
  %323 = load ptr, ptr %17, align 8
  call void @free(ptr %323)
  br label %rl_m_drop__String.exit71

rl_m_drop__String.exit71:                         ; preds = %rl_m_drop__String.exit69, %322
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %288, i8 0, i64 16, i1 false)
  %324 = load i64, ptr %289, align 8
  %.not2.i.i72 = icmp eq i64 %324, 0
  br i1 %.not2.i.i72, label %rl_m_drop__String.exit73, label %325

325:                                              ; preds = %rl_m_drop__String.exit71
  %326 = load ptr, ptr %16, align 8
  call void @free(ptr %326)
  br label %rl_m_drop__String.exit73

rl_m_drop__String.exit73:                         ; preds = %rl_m_drop__String.exit71, %325
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %290, i8 0, i64 16, i1 false)
  %327 = load i64, ptr %291, align 8
  %.not2.i.i74 = icmp eq i64 %327, 0
  br i1 %.not2.i.i74, label %rl_m_drop__String.exit75, label %328

328:                                              ; preds = %rl_m_drop__String.exit73
  %329 = load ptr, ptr %14, align 8
  call void @free(ptr %329)
  br label %rl_m_drop__String.exit75

rl_m_drop__String.exit75:                         ; preds = %rl_m_drop__String.exit73, %328
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %292, i8 0, i64 16, i1 false)
  %330 = load i64, ptr %293, align 8
  %.not2.i.i76 = icmp eq i64 %330, 0
  br i1 %.not2.i.i76, label %rl_m_drop__String.exit77, label %331

331:                                              ; preds = %rl_m_drop__String.exit75
  %332 = load ptr, ptr %13, align 8
  call void @free(ptr %332)
  br label %rl_m_drop__String.exit77

rl_m_drop__String.exit77:                         ; preds = %rl_m_drop__String.exit75, %331
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %294, i8 0, i64 16, i1 false)
  %.not5 = icmp eq i64 %301, %272
  br i1 %.not5, label %._crit_edge142, label %295

._crit_edge142:                                   ; preds = %rl_m_drop__String.exit77, %._crit_edge
  %333 = load ptr, ptr %53, align 8
  call void @free(ptr %333)
  %334 = load i64, ptr %187, align 8
  %.not.i80 = icmp eq i64 %334, 0
  br i1 %.not.i80, label %rl_m_drop__DictTint64_tTint64_tT.exit82, label %335

335:                                              ; preds = %._crit_edge142
  %336 = load ptr, ptr %34, align 8
  call void @free(ptr %336)
  br label %rl_m_drop__DictTint64_tTint64_tT.exit82

rl_m_drop__DictTint64_tTint64_tT.exit82:          ; preds = %._crit_edge142, %335
  %337 = getelementptr inbounds i8, ptr %36, i64 16
  %338 = load i64, ptr %337, align 8
  %.not2.i = icmp eq i64 %338, 0
  br i1 %.not2.i, label %rl_m_drop__VectorTint64_tT.exit, label %339

339:                                              ; preds = %rl_m_drop__DictTint64_tTint64_tT.exit82
  %340 = load ptr, ptr %36, align 8
  call void @free(ptr %340)
  br label %rl_m_drop__VectorTint64_tT.exit

rl_m_drop__VectorTint64_tT.exit:                  ; preds = %rl_m_drop__DictTint64_tTint64_tT.exit82, %339
  %341 = getelementptr inbounds i8, ptr %35, i64 16
  %342 = load i64, ptr %341, align 8
  %.not2.i83 = icmp eq i64 %342, 0
  br i1 %.not2.i83, label %rl_m_drop__DictTint64_tTint64_tT.exit86, label %343

343:                                              ; preds = %rl_m_drop__VectorTint64_tT.exit
  %344 = load ptr, ptr %35, align 8
  call void @free(ptr %344)
  br label %rl_m_drop__DictTint64_tTint64_tT.exit86

rl_m_drop__DictTint64_tTint64_tT.exit86:          ; preds = %343, %rl_m_drop__VectorTint64_tT.exit
  store i64 0, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #10

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #11

; Function Attrs: cold noreturn nounwind memory(inaccessiblemem: write)
declare void @llvm.trap() #12

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smax.i64(i64, i64) #13

; Function Attrs: nofree nounwind willreturn allockind("alloc,zeroed") allocsize(0,1) memory(inaccessiblemem: readwrite)
declare noalias noundef ptr @calloc(i64 noundef, i64 noundef) local_unnamed_addr #14

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #15

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #15

attributes #0 = { mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite) "alloc-family"="malloc" }
attributes #1 = { nofree nounwind }
attributes #2 = { mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite) "alloc-family"="malloc" }
attributes #3 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write) }
attributes #4 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #5 = { nounwind }
attributes #6 = { mustprogress nounwind willreturn }
attributes #7 = { nofree nounwind memory(write, argmem: readwrite, inaccessiblemem: readwrite) }
attributes #8 = { nofree norecurse nosync nounwind memory(write, argmem: readwrite, inaccessiblemem: none) }
attributes #9 = { nofree norecurse nosync nounwind memory(read, argmem: readwrite, inaccessiblemem: none) }
attributes #10 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #11 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #12 = { cold noreturn nounwind memory(inaccessiblemem: write) }
attributes #13 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #14 = { nofree nounwind willreturn allockind("alloc,zeroed") allocsize(0,1) memory(inaccessiblemem: readwrite) "alloc-family"="malloc" }
attributes #15 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = distinct !{!1, !2, !3}
!2 = !{!"llvm.loop.isvectorized", i32 1}
!3 = !{!"llvm.loop.unroll.runtime.disable"}
!4 = distinct !{!4, !2}
!5 = distinct !{!5, !2, !3}
!6 = distinct !{!6, !2}
!7 = distinct !{!7, !2, !3}
!8 = distinct !{!8, !2}
!9 = distinct !{!9, !2, !3}
!10 = distinct !{!10, !2}
!11 = distinct !{!11, !2, !3}
!12 = distinct !{!12, !2}
!13 = distinct !{!13, !2, !3}
!14 = distinct !{!14, !2}
!15 = distinct !{!15, !2, !3}
!16 = distinct !{!16, !2}
!17 = distinct !{!17, !2, !3}
!18 = distinct !{!18, !2}
!19 = distinct !{!19, !2, !3}
!20 = distinct !{!20, !2}
!21 = distinct !{!21, !22}
!22 = !{!"llvm.loop.peeled.count", i32 1}
!23 = distinct !{!23, !2, !3}
!24 = distinct !{!24, !2}
!25 = distinct !{!25, !2, !3}
!26 = distinct !{!26, !2}
!27 = distinct !{!27, !2, !3}
!28 = distinct !{!28, !2}
!29 = distinct !{!29, !2, !3}
!30 = distinct !{!30, !2}
!31 = distinct !{!31, !2, !3}
!32 = distinct !{!32, !2}
!33 = distinct !{!33, !2, !3}
!34 = distinct !{!34, !2}
!35 = distinct !{!35, !2, !3}
!36 = distinct !{!36, !2}
!37 = distinct !{!37, !2, !3}
!38 = distinct !{!38, !2}
!39 = distinct !{!39, !2, !3}
!40 = distinct !{!40, !2}
!41 = distinct !{!41, !2, !3}
!42 = distinct !{!42, !2}
!43 = distinct !{!43, !2, !3}
!44 = distinct !{!44, !2}
!45 = distinct !{!45, !2, !3}
!46 = distinct !{!46, !2}
