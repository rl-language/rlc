; ModuleID = 'dict.ll'
source_filename = "dict.rl"
target datalayout = "S128-e-p272:64:64:64:64-p271:32:32:32:32-i64:64-f80:128-i128:128-i1:8-p0:64:64:64:64-i16:16-i8:8-i32:32-f128:128-f16:16-p270:32:32:32:32-f64:64"
target triple = "x86_64-unknown-linux-gnu"

%Entry = type { i8, i64, i64, i64 }
%String = type { %Vector }
%Vector = type { ptr, i64, i64 }
%Dict = type { ptr, i64, i64, double }
%Vector.1 = type { ptr, i64, i64 }

@str_23 = internal constant [19 x i8] c"dicExc: key, value\00"
@str_22 = internal constant [3 x i8] c", \00"
@str_21 = internal constant [16 x i8] c"dic: key, value\00"
@str_20 = internal constant [15 x i8] c"REMOVAL FAILED\00"
@str_19 = internal constant [7 x i8] c"value \00"
@str_18 = internal constant [22 x i8] c"Found entry with key \00"
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
  %.not3.i = icmp eq i64 %3, 0
  br i1 %.not3.i, label %rl_m_drop__VectorTint8_tT.exit, label %4

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
  %.08 = phi i64 [ %13, %.lr.ph ], [ 2166136261, %.lr.ph.preheader.split ]
  %storemerge7 = phi i64 [ %14, %.lr.ph ], [ 0, %.lr.ph.preheader.split ]
  %8 = getelementptr i8, ptr %.pre, i64 %storemerge7
  %9 = load i8, ptr %8, align 1
  %10 = sext i8 %9 to i64
  %11 = xor i64 %.08, %10
  %12 = mul i64 %11, 16777619
  %13 = and i64 %12, 9223372036854775807
  %14 = add nuw nsw i64 %storemerge7, 1
  %15 = icmp slt i64 %14, %5
  br i1 %15, label %.lr.ph, label %._crit_edge

16:                                               ; preds = %.lr.ph.preheader
  %17 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

._crit_edge:                                      ; preds = %.lr.ph, %2
  %.0.lcssa = phi i64 [ 2166136261, %2 ], [ %13, %.lr.ph ]
  store i64 %.0.lcssa, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @rl_compute_hash_of__int64_t_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #4 {
  %.val = load i64, ptr %1, align 8
  %3 = lshr i64 %.val, 33
  %4 = xor i64 %3, %.val
  %5 = mul i64 %4, 1099511628211
  %6 = lshr i64 %5, 33
  %7 = xor i64 %6, %5
  %8 = mul i64 %7, 16777619
  %9 = lshr i64 %8, 33
  %.masked.i.i = and i64 %8, 9223372036854775807
  %10 = xor i64 %.masked.i.i, %9
  store i64 %10, ptr %0, align 1
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
  %.val = load i64, ptr %1, align 8
  %.val1 = load i64, ptr %2, align 8
  %4 = icmp eq i64 %.val, %.val1
  %5 = zext i1 %4 to i8
  store i8 %5, ptr %0, align 1
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
  %.056 = phi i64 [ %7, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %5 = load ptr, ptr %0, align 8
  %6 = getelementptr %Entry, ptr %5, i64 %.056
  store i8 0, ptr %6, align 1
  %7 = add nuw nsw i64 %.056, 1
  %8 = load i64, ptr %1, align 8
  %9 = icmp slt i64 %7, %8
  br i1 %9, label %.lr.ph, label %._crit_edge

._crit_edge:                                      ; preds = %.lr.ph
  ret void
}

; Function Attrs: nounwind
define void @rl_m_values__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
rl_m_init__VectorTint64_tT.exit.preheader:
  %calloc37 = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  %2 = getelementptr i8, ptr %1, i64 8
  %3 = load i64, ptr %2, align 8
  %4 = icmp sgt i64 %3, 0
  br i1 %4, label %.lr.ph, label %.lr.ph.i6.preheader

.lr.ph.i6.preheader.loopexit:                     ; preds = %rl_m_init__VectorTint64_tT.exit
  %5 = icmp eq i64 %.sroa.15.2, 0
  br label %.lr.ph.i6.preheader

.lr.ph.i6.preheader:                              ; preds = %.lr.ph.i6.preheader.loopexit, %rl_m_init__VectorTint64_tT.exit.preheader
  %.sroa.7.0.lcssa = phi i64 [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %.sroa.7.1, %.lr.ph.i6.preheader.loopexit ]
  %.sroa.15.0.lcssa = phi i1 [ false, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %5, %.lr.ph.i6.preheader.loopexit ]
  %.sroa.0.0.lcssa = phi ptr [ %calloc37, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %.sroa.0.2, %.lr.ph.i6.preheader.loopexit ]
  %calloc = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  %6 = icmp sgt i64 %.sroa.7.0.lcssa, 0
  br i1 %6, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i, label %rl_m_assign__VectorTint64_tT_VectorTint64_tT.exit

.lr.ph:                                           ; preds = %rl_m_init__VectorTint64_tT.exit.preheader, %rl_m_init__VectorTint64_tT.exit
  %7 = phi i64 [ %37, %rl_m_init__VectorTint64_tT.exit ], [ %3, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.033 = phi i64 [ %.1, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.0532 = phi i64 [ %38, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.sroa.0.031 = phi ptr [ %.sroa.0.2, %rl_m_init__VectorTint64_tT.exit ], [ %calloc37, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.sroa.15.030 = phi i64 [ %.sroa.15.2, %rl_m_init__VectorTint64_tT.exit ], [ 4, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.sroa.7.029 = phi i64 [ %.sroa.7.1, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.sroa.0.03138 = ptrtoint ptr %.sroa.0.031 to i64
  %8 = load ptr, ptr %1, align 8
  %9 = getelementptr %Entry, ptr %8, i64 %.0532
  %10 = load i8, ptr %9, align 1
  %.not = icmp eq i8 %10, 0
  br i1 %.not, label %rl_m_init__VectorTint64_tT.exit, label %11

11:                                               ; preds = %.lr.ph
  %12 = getelementptr i8, ptr %9, i64 24
  %13 = add i64 %.sroa.7.029, 1
  %14 = icmp sgt i64 %.sroa.15.030, %13
  br i1 %14, label %rl_m_append__VectorTint64_tT_int64_t.exit, label %15

15:                                               ; preds = %11
  %16 = shl i64 %13, 4
  %17 = tail call ptr @malloc(i64 %16)
  %18 = ptrtoint ptr %17 to i64
  %19 = trunc i64 %13 to i63
  %20 = icmp sgt i63 %19, 0
  br i1 %20, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %15
  tail call void @llvm.memset.p0.i64(ptr align 8 %17, i8 0, i64 %16, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %15
  %21 = icmp sgt i64 %.sroa.7.029, 0
  br i1 %21, label %.lr.ph15.i.i.preheader, label %.preheader.i.i

.lr.ph15.i.i.preheader:                           ; preds = %.preheader12.i.i
  %min.iters.check = icmp ult i64 %.sroa.7.029, 4
  %22 = sub i64 %18, %.sroa.0.03138
  %diff.check = icmp ult i64 %22, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader58, label %vector.ph

.lr.ph15.i.i.preheader58:                         ; preds = %middle.block, %.lr.ph15.i.i.preheader
  %.114.i.i.ph = phi i64 [ 0, %.lr.ph15.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph15.i.i

vector.ph:                                        ; preds = %.lr.ph15.i.i.preheader
  %n.vec = and i64 %.sroa.7.029, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %23 = getelementptr i64, ptr %17, i64 %index
  %24 = getelementptr i64, ptr %.sroa.0.031, i64 %index
  %25 = getelementptr i8, ptr %24, i64 16
  %wide.load = load <2 x i64>, ptr %24, align 8
  %wide.load39 = load <2 x i64>, ptr %25, align 8
  %26 = getelementptr i8, ptr %23, i64 16
  store <2 x i64> %wide.load, ptr %23, align 8
  store <2 x i64> %wide.load39, ptr %26, align 8
  %index.next = add nuw i64 %index, 4
  %27 = icmp eq i64 %index.next, %n.vec
  br i1 %27, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %.sroa.7.029, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph15.i.i.preheader58

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %.preheader12.i.i
  tail call void @free(ptr %.sroa.0.031)
  %28 = shl i64 %13, 1
  br label %rl_m_append__VectorTint64_tT_int64_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader58, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %32, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader58 ]
  %29 = getelementptr i64, ptr %17, i64 %.114.i.i
  %30 = getelementptr i64, ptr %.sroa.0.031, i64 %.114.i.i
  %31 = load i64, ptr %30, align 8
  store i64 %31, ptr %29, align 8
  %32 = add nuw nsw i64 %.114.i.i, 1
  %33 = icmp slt i64 %32, %.sroa.7.029
  br i1 %33, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !4

rl_m_append__VectorTint64_tT_int64_t.exit:        ; preds = %11, %.preheader.i.i
  %.sroa.15.1 = phi i64 [ %28, %.preheader.i.i ], [ %.sroa.15.030, %11 ]
  %.sroa.0.1 = phi ptr [ %17, %.preheader.i.i ], [ %.sroa.0.031, %11 ]
  %34 = getelementptr i64, ptr %.sroa.0.1, i64 %.sroa.7.029
  %35 = load i64, ptr %12, align 8
  store i64 %35, ptr %34, align 8
  %36 = add i64 %.033, 1
  %.pre = load i64, ptr %2, align 8
  br label %rl_m_init__VectorTint64_tT.exit

rl_m_init__VectorTint64_tT.exit:                  ; preds = %.lr.ph, %rl_m_append__VectorTint64_tT_int64_t.exit
  %37 = phi i64 [ %7, %.lr.ph ], [ %.pre, %rl_m_append__VectorTint64_tT_int64_t.exit ]
  %.sroa.7.1 = phi i64 [ %.sroa.7.029, %.lr.ph ], [ %13, %rl_m_append__VectorTint64_tT_int64_t.exit ]
  %.sroa.15.2 = phi i64 [ %.sroa.15.030, %.lr.ph ], [ %.sroa.15.1, %rl_m_append__VectorTint64_tT_int64_t.exit ]
  %.sroa.0.2 = phi ptr [ %.sroa.0.031, %.lr.ph ], [ %.sroa.0.1, %rl_m_append__VectorTint64_tT_int64_t.exit ]
  %.1 = phi i64 [ %.033, %.lr.ph ], [ %36, %rl_m_append__VectorTint64_tT_int64_t.exit ]
  %38 = add i64 %.0532, 1
  %39 = icmp slt i64 %.1, %37
  br i1 %39, label %.lr.ph, label %.lr.ph.i6.preheader.loopexit

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i: ; preds = %.lr.ph.i6.preheader, %rl_m_append__VectorTint64_tT_int64_t.exit.i
  %.sroa.1524.0 = phi i64 [ %.sroa.1524.1, %rl_m_append__VectorTint64_tT_int64_t.exit.i ], [ 4, %.lr.ph.i6.preheader ]
  %.sroa.9.0 = phi i64 [ %41, %rl_m_append__VectorTint64_tT_int64_t.exit.i ], [ 0, %.lr.ph.i6.preheader ]
  %.sroa.021.0 = phi ptr [ %.sroa.021.1, %rl_m_append__VectorTint64_tT_int64_t.exit.i ], [ %calloc, %.lr.ph.i6.preheader ]
  %.sroa.021.041 = ptrtoint ptr %.sroa.021.0 to i64
  %40 = getelementptr i64, ptr %.sroa.0.0.lcssa, i64 %.sroa.9.0
  %41 = add nuw nsw i64 %.sroa.9.0, 1
  %42 = icmp sgt i64 %.sroa.1524.0, %41
  br i1 %42, label %rl_m_append__VectorTint64_tT_int64_t.exit.i, label %43

43:                                               ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i
  %44 = shl i64 %41, 4
  %45 = tail call ptr @malloc(i64 %44)
  %46 = ptrtoint ptr %45 to i64
  %47 = trunc nuw i64 %41 to i63
  %48 = icmp sgt i63 %47, 0
  br i1 %48, label %.lr.ph.preheader.i.i.i, label %.preheader12.i.i.i

.lr.ph.preheader.i.i.i:                           ; preds = %43
  tail call void @llvm.memset.p0.i64(ptr align 8 %45, i8 0, i64 %44, i1 false)
  br label %.preheader12.i.i.i

.preheader12.i.i.i:                               ; preds = %.lr.ph.preheader.i.i.i, %43
  %.not36 = icmp eq i64 %.sroa.9.0, 0
  br i1 %.not36, label %.preheader.i.i.i, label %.lr.ph15.i.i.i.preheader

.lr.ph15.i.i.i.preheader:                         ; preds = %.preheader12.i.i.i
  %min.iters.check45 = icmp ult i64 %.sroa.9.0, 4
  %49 = sub i64 %46, %.sroa.021.041
  %diff.check42 = icmp ult i64 %49, 32
  %or.cond56 = or i1 %min.iters.check45, %diff.check42
  br i1 %or.cond56, label %.lr.ph15.i.i.i.preheader57, label %vector.ph46

.lr.ph15.i.i.i.preheader57:                       ; preds = %middle.block43, %.lr.ph15.i.i.i.preheader
  %.114.i.i.i.ph = phi i64 [ 0, %.lr.ph15.i.i.i.preheader ], [ %n.vec48, %middle.block43 ]
  br label %.lr.ph15.i.i.i

vector.ph46:                                      ; preds = %.lr.ph15.i.i.i.preheader
  %n.vec48 = and i64 %.sroa.9.0, 9223372036854775804
  br label %vector.body50

vector.body50:                                    ; preds = %vector.body50, %vector.ph46
  %index51 = phi i64 [ 0, %vector.ph46 ], [ %index.next54, %vector.body50 ]
  %50 = getelementptr i64, ptr %45, i64 %index51
  %51 = getelementptr i64, ptr %.sroa.021.0, i64 %index51
  %52 = getelementptr i8, ptr %51, i64 16
  %wide.load52 = load <2 x i64>, ptr %51, align 8
  %wide.load53 = load <2 x i64>, ptr %52, align 8
  %53 = getelementptr i8, ptr %50, i64 16
  store <2 x i64> %wide.load52, ptr %50, align 8
  store <2 x i64> %wide.load53, ptr %53, align 8
  %index.next54 = add nuw i64 %index51, 4
  %54 = icmp eq i64 %index.next54, %n.vec48
  br i1 %54, label %middle.block43, label %vector.body50, !llvm.loop !5

middle.block43:                                   ; preds = %vector.body50
  %cmp.n55 = icmp eq i64 %.sroa.9.0, %n.vec48
  br i1 %cmp.n55, label %.preheader.i.i.i, label %.lr.ph15.i.i.i.preheader57

.preheader.i.i.i:                                 ; preds = %.lr.ph15.i.i.i, %middle.block43, %.preheader12.i.i.i
  tail call void @free(ptr nonnull %.sroa.021.0)
  %55 = shl nuw i64 %41, 1
  br label %rl_m_append__VectorTint64_tT_int64_t.exit.i

.lr.ph15.i.i.i:                                   ; preds = %.lr.ph15.i.i.i.preheader57, %.lr.ph15.i.i.i
  %.114.i.i.i = phi i64 [ %59, %.lr.ph15.i.i.i ], [ %.114.i.i.i.ph, %.lr.ph15.i.i.i.preheader57 ]
  %56 = getelementptr i64, ptr %45, i64 %.114.i.i.i
  %57 = getelementptr i64, ptr %.sroa.021.0, i64 %.114.i.i.i
  %58 = load i64, ptr %57, align 8
  store i64 %58, ptr %56, align 8
  %59 = add nuw nsw i64 %.114.i.i.i, 1
  %60 = icmp ult i64 %59, %.sroa.9.0
  br i1 %60, label %.lr.ph15.i.i.i, label %.preheader.i.i.i, !llvm.loop !6

rl_m_append__VectorTint64_tT_int64_t.exit.i:      ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i, %.preheader.i.i.i
  %.sroa.1524.1 = phi i64 [ %55, %.preheader.i.i.i ], [ %.sroa.1524.0, %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i ]
  %.sroa.021.1 = phi ptr [ %45, %.preheader.i.i.i ], [ %.sroa.021.0, %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i ]
  %61 = getelementptr i64, ptr %.sroa.021.1, i64 %.sroa.9.0
  %62 = load i64, ptr %40, align 8
  store i64 %62, ptr %61, align 8
  %63 = icmp slt i64 %41, %.sroa.7.0.lcssa
  br i1 %63, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i, label %rl_m_assign__VectorTint64_tT_VectorTint64_tT.exit

rl_m_assign__VectorTint64_tT_VectorTint64_tT.exit: ; preds = %rl_m_append__VectorTint64_tT_int64_t.exit.i, %.lr.ph.i6.preheader
  %.sroa.1524.2 = phi i64 [ 4, %.lr.ph.i6.preheader ], [ %.sroa.1524.1, %rl_m_append__VectorTint64_tT_int64_t.exit.i ]
  %.sroa.9.1 = phi i64 [ 0, %.lr.ph.i6.preheader ], [ %.sroa.7.0.lcssa, %rl_m_append__VectorTint64_tT_int64_t.exit.i ]
  %.sroa.021.2 = phi ptr [ %calloc, %.lr.ph.i6.preheader ], [ %.sroa.021.1, %rl_m_append__VectorTint64_tT_int64_t.exit.i ]
  br i1 %.sroa.15.0.lcssa, label %rl_m_drop__VectorTint64_tT.exit, label %64

64:                                               ; preds = %rl_m_assign__VectorTint64_tT_VectorTint64_tT.exit
  tail call void @free(ptr %.sroa.0.0.lcssa)
  br label %rl_m_drop__VectorTint64_tT.exit

rl_m_drop__VectorTint64_tT.exit:                  ; preds = %rl_m_assign__VectorTint64_tT_VectorTint64_tT.exit, %64
  store ptr %.sroa.021.2, ptr %0, align 1
  %.sroa.2.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 8
  store i64 %.sroa.9.1, ptr %.sroa.2.0..sroa_idx, align 1
  %.sroa.3.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 16
  store i64 %.sroa.1524.2, ptr %.sroa.3.0..sroa_idx, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_keys__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
rl_m_init__VectorTint64_tT.exit.preheader:
  %calloc37 = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  %2 = getelementptr i8, ptr %1, i64 8
  %3 = load i64, ptr %2, align 8
  %4 = icmp sgt i64 %3, 0
  br i1 %4, label %.lr.ph, label %.lr.ph.i6.preheader

.lr.ph.i6.preheader.loopexit:                     ; preds = %rl_m_init__VectorTint64_tT.exit
  %5 = icmp eq i64 %.sroa.15.2, 0
  br label %.lr.ph.i6.preheader

.lr.ph.i6.preheader:                              ; preds = %.lr.ph.i6.preheader.loopexit, %rl_m_init__VectorTint64_tT.exit.preheader
  %.sroa.7.0.lcssa = phi i64 [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %.sroa.7.1, %.lr.ph.i6.preheader.loopexit ]
  %.sroa.15.0.lcssa = phi i1 [ false, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %5, %.lr.ph.i6.preheader.loopexit ]
  %.sroa.0.0.lcssa = phi ptr [ %calloc37, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %.sroa.0.2, %.lr.ph.i6.preheader.loopexit ]
  %calloc = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  %6 = icmp sgt i64 %.sroa.7.0.lcssa, 0
  br i1 %6, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i, label %rl_m_assign__VectorTint64_tT_VectorTint64_tT.exit

.lr.ph:                                           ; preds = %rl_m_init__VectorTint64_tT.exit.preheader, %rl_m_init__VectorTint64_tT.exit
  %7 = phi i64 [ %37, %rl_m_init__VectorTint64_tT.exit ], [ %3, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.033 = phi i64 [ %.1, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.0532 = phi i64 [ %38, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.sroa.0.031 = phi ptr [ %.sroa.0.2, %rl_m_init__VectorTint64_tT.exit ], [ %calloc37, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.sroa.15.030 = phi i64 [ %.sroa.15.2, %rl_m_init__VectorTint64_tT.exit ], [ 4, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.sroa.7.029 = phi i64 [ %.sroa.7.1, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.sroa.0.03138 = ptrtoint ptr %.sroa.0.031 to i64
  %8 = load ptr, ptr %1, align 8
  %9 = getelementptr %Entry, ptr %8, i64 %.0532
  %10 = load i8, ptr %9, align 1
  %.not = icmp eq i8 %10, 0
  br i1 %.not, label %rl_m_init__VectorTint64_tT.exit, label %11

11:                                               ; preds = %.lr.ph
  %12 = getelementptr i8, ptr %9, i64 16
  %13 = add i64 %.sroa.7.029, 1
  %14 = icmp sgt i64 %.sroa.15.030, %13
  br i1 %14, label %rl_m_append__VectorTint64_tT_int64_t.exit, label %15

15:                                               ; preds = %11
  %16 = shl i64 %13, 4
  %17 = tail call ptr @malloc(i64 %16)
  %18 = ptrtoint ptr %17 to i64
  %19 = trunc i64 %13 to i63
  %20 = icmp sgt i63 %19, 0
  br i1 %20, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %15
  tail call void @llvm.memset.p0.i64(ptr align 8 %17, i8 0, i64 %16, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %15
  %21 = icmp sgt i64 %.sroa.7.029, 0
  br i1 %21, label %.lr.ph15.i.i.preheader, label %.preheader.i.i

.lr.ph15.i.i.preheader:                           ; preds = %.preheader12.i.i
  %min.iters.check = icmp ult i64 %.sroa.7.029, 4
  %22 = sub i64 %18, %.sroa.0.03138
  %diff.check = icmp ult i64 %22, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader58, label %vector.ph

.lr.ph15.i.i.preheader58:                         ; preds = %middle.block, %.lr.ph15.i.i.preheader
  %.114.i.i.ph = phi i64 [ 0, %.lr.ph15.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph15.i.i

vector.ph:                                        ; preds = %.lr.ph15.i.i.preheader
  %n.vec = and i64 %.sroa.7.029, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %23 = getelementptr i64, ptr %17, i64 %index
  %24 = getelementptr i64, ptr %.sroa.0.031, i64 %index
  %25 = getelementptr i8, ptr %24, i64 16
  %wide.load = load <2 x i64>, ptr %24, align 8
  %wide.load39 = load <2 x i64>, ptr %25, align 8
  %26 = getelementptr i8, ptr %23, i64 16
  store <2 x i64> %wide.load, ptr %23, align 8
  store <2 x i64> %wide.load39, ptr %26, align 8
  %index.next = add nuw i64 %index, 4
  %27 = icmp eq i64 %index.next, %n.vec
  br i1 %27, label %middle.block, label %vector.body, !llvm.loop !7

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %.sroa.7.029, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph15.i.i.preheader58

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %.preheader12.i.i
  tail call void @free(ptr %.sroa.0.031)
  %28 = shl i64 %13, 1
  br label %rl_m_append__VectorTint64_tT_int64_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader58, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %32, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader58 ]
  %29 = getelementptr i64, ptr %17, i64 %.114.i.i
  %30 = getelementptr i64, ptr %.sroa.0.031, i64 %.114.i.i
  %31 = load i64, ptr %30, align 8
  store i64 %31, ptr %29, align 8
  %32 = add nuw nsw i64 %.114.i.i, 1
  %33 = icmp slt i64 %32, %.sroa.7.029
  br i1 %33, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !8

rl_m_append__VectorTint64_tT_int64_t.exit:        ; preds = %11, %.preheader.i.i
  %.sroa.15.1 = phi i64 [ %28, %.preheader.i.i ], [ %.sroa.15.030, %11 ]
  %.sroa.0.1 = phi ptr [ %17, %.preheader.i.i ], [ %.sroa.0.031, %11 ]
  %34 = getelementptr i64, ptr %.sroa.0.1, i64 %.sroa.7.029
  %35 = load i64, ptr %12, align 8
  store i64 %35, ptr %34, align 8
  %36 = add i64 %.033, 1
  %.pre = load i64, ptr %2, align 8
  br label %rl_m_init__VectorTint64_tT.exit

rl_m_init__VectorTint64_tT.exit:                  ; preds = %.lr.ph, %rl_m_append__VectorTint64_tT_int64_t.exit
  %37 = phi i64 [ %7, %.lr.ph ], [ %.pre, %rl_m_append__VectorTint64_tT_int64_t.exit ]
  %.sroa.7.1 = phi i64 [ %.sroa.7.029, %.lr.ph ], [ %13, %rl_m_append__VectorTint64_tT_int64_t.exit ]
  %.sroa.15.2 = phi i64 [ %.sroa.15.030, %.lr.ph ], [ %.sroa.15.1, %rl_m_append__VectorTint64_tT_int64_t.exit ]
  %.sroa.0.2 = phi ptr [ %.sroa.0.031, %.lr.ph ], [ %.sroa.0.1, %rl_m_append__VectorTint64_tT_int64_t.exit ]
  %.1 = phi i64 [ %.033, %.lr.ph ], [ %36, %rl_m_append__VectorTint64_tT_int64_t.exit ]
  %38 = add i64 %.0532, 1
  %39 = icmp slt i64 %.1, %37
  br i1 %39, label %.lr.ph, label %.lr.ph.i6.preheader.loopexit

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i: ; preds = %.lr.ph.i6.preheader, %rl_m_append__VectorTint64_tT_int64_t.exit.i
  %.sroa.1524.0 = phi i64 [ %.sroa.1524.1, %rl_m_append__VectorTint64_tT_int64_t.exit.i ], [ 4, %.lr.ph.i6.preheader ]
  %.sroa.9.0 = phi i64 [ %41, %rl_m_append__VectorTint64_tT_int64_t.exit.i ], [ 0, %.lr.ph.i6.preheader ]
  %.sroa.021.0 = phi ptr [ %.sroa.021.1, %rl_m_append__VectorTint64_tT_int64_t.exit.i ], [ %calloc, %.lr.ph.i6.preheader ]
  %.sroa.021.041 = ptrtoint ptr %.sroa.021.0 to i64
  %40 = getelementptr i64, ptr %.sroa.0.0.lcssa, i64 %.sroa.9.0
  %41 = add nuw nsw i64 %.sroa.9.0, 1
  %42 = icmp sgt i64 %.sroa.1524.0, %41
  br i1 %42, label %rl_m_append__VectorTint64_tT_int64_t.exit.i, label %43

43:                                               ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i
  %44 = shl i64 %41, 4
  %45 = tail call ptr @malloc(i64 %44)
  %46 = ptrtoint ptr %45 to i64
  %47 = trunc nuw i64 %41 to i63
  %48 = icmp sgt i63 %47, 0
  br i1 %48, label %.lr.ph.preheader.i.i.i, label %.preheader12.i.i.i

.lr.ph.preheader.i.i.i:                           ; preds = %43
  tail call void @llvm.memset.p0.i64(ptr align 8 %45, i8 0, i64 %44, i1 false)
  br label %.preheader12.i.i.i

.preheader12.i.i.i:                               ; preds = %.lr.ph.preheader.i.i.i, %43
  %.not36 = icmp eq i64 %.sroa.9.0, 0
  br i1 %.not36, label %.preheader.i.i.i, label %.lr.ph15.i.i.i.preheader

.lr.ph15.i.i.i.preheader:                         ; preds = %.preheader12.i.i.i
  %min.iters.check45 = icmp ult i64 %.sroa.9.0, 4
  %49 = sub i64 %46, %.sroa.021.041
  %diff.check42 = icmp ult i64 %49, 32
  %or.cond56 = or i1 %min.iters.check45, %diff.check42
  br i1 %or.cond56, label %.lr.ph15.i.i.i.preheader57, label %vector.ph46

.lr.ph15.i.i.i.preheader57:                       ; preds = %middle.block43, %.lr.ph15.i.i.i.preheader
  %.114.i.i.i.ph = phi i64 [ 0, %.lr.ph15.i.i.i.preheader ], [ %n.vec48, %middle.block43 ]
  br label %.lr.ph15.i.i.i

vector.ph46:                                      ; preds = %.lr.ph15.i.i.i.preheader
  %n.vec48 = and i64 %.sroa.9.0, 9223372036854775804
  br label %vector.body50

vector.body50:                                    ; preds = %vector.body50, %vector.ph46
  %index51 = phi i64 [ 0, %vector.ph46 ], [ %index.next54, %vector.body50 ]
  %50 = getelementptr i64, ptr %45, i64 %index51
  %51 = getelementptr i64, ptr %.sroa.021.0, i64 %index51
  %52 = getelementptr i8, ptr %51, i64 16
  %wide.load52 = load <2 x i64>, ptr %51, align 8
  %wide.load53 = load <2 x i64>, ptr %52, align 8
  %53 = getelementptr i8, ptr %50, i64 16
  store <2 x i64> %wide.load52, ptr %50, align 8
  store <2 x i64> %wide.load53, ptr %53, align 8
  %index.next54 = add nuw i64 %index51, 4
  %54 = icmp eq i64 %index.next54, %n.vec48
  br i1 %54, label %middle.block43, label %vector.body50, !llvm.loop !9

middle.block43:                                   ; preds = %vector.body50
  %cmp.n55 = icmp eq i64 %.sroa.9.0, %n.vec48
  br i1 %cmp.n55, label %.preheader.i.i.i, label %.lr.ph15.i.i.i.preheader57

.preheader.i.i.i:                                 ; preds = %.lr.ph15.i.i.i, %middle.block43, %.preheader12.i.i.i
  tail call void @free(ptr nonnull %.sroa.021.0)
  %55 = shl nuw i64 %41, 1
  br label %rl_m_append__VectorTint64_tT_int64_t.exit.i

.lr.ph15.i.i.i:                                   ; preds = %.lr.ph15.i.i.i.preheader57, %.lr.ph15.i.i.i
  %.114.i.i.i = phi i64 [ %59, %.lr.ph15.i.i.i ], [ %.114.i.i.i.ph, %.lr.ph15.i.i.i.preheader57 ]
  %56 = getelementptr i64, ptr %45, i64 %.114.i.i.i
  %57 = getelementptr i64, ptr %.sroa.021.0, i64 %.114.i.i.i
  %58 = load i64, ptr %57, align 8
  store i64 %58, ptr %56, align 8
  %59 = add nuw nsw i64 %.114.i.i.i, 1
  %60 = icmp ult i64 %59, %.sroa.9.0
  br i1 %60, label %.lr.ph15.i.i.i, label %.preheader.i.i.i, !llvm.loop !10

rl_m_append__VectorTint64_tT_int64_t.exit.i:      ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i, %.preheader.i.i.i
  %.sroa.1524.1 = phi i64 [ %55, %.preheader.i.i.i ], [ %.sroa.1524.0, %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i ]
  %.sroa.021.1 = phi ptr [ %45, %.preheader.i.i.i ], [ %.sroa.021.0, %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i ]
  %61 = getelementptr i64, ptr %.sroa.021.1, i64 %.sroa.9.0
  %62 = load i64, ptr %40, align 8
  store i64 %62, ptr %61, align 8
  %63 = icmp slt i64 %41, %.sroa.7.0.lcssa
  br i1 %63, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit.i, label %rl_m_assign__VectorTint64_tT_VectorTint64_tT.exit

rl_m_assign__VectorTint64_tT_VectorTint64_tT.exit: ; preds = %rl_m_append__VectorTint64_tT_int64_t.exit.i, %.lr.ph.i6.preheader
  %.sroa.1524.2 = phi i64 [ 4, %.lr.ph.i6.preheader ], [ %.sroa.1524.1, %rl_m_append__VectorTint64_tT_int64_t.exit.i ]
  %.sroa.9.1 = phi i64 [ 0, %.lr.ph.i6.preheader ], [ %.sroa.7.0.lcssa, %rl_m_append__VectorTint64_tT_int64_t.exit.i ]
  %.sroa.021.2 = phi ptr [ %calloc, %.lr.ph.i6.preheader ], [ %.sroa.021.1, %rl_m_append__VectorTint64_tT_int64_t.exit.i ]
  br i1 %.sroa.15.0.lcssa, label %rl_m_drop__VectorTint64_tT.exit, label %64

64:                                               ; preds = %rl_m_assign__VectorTint64_tT_VectorTint64_tT.exit
  tail call void @free(ptr %.sroa.0.0.lcssa)
  br label %rl_m_drop__VectorTint64_tT.exit

rl_m_drop__VectorTint64_tT.exit:                  ; preds = %rl_m_assign__VectorTint64_tT_VectorTint64_tT.exit, %64
  store ptr %.sroa.021.2, ptr %0, align 1
  %.sroa.2.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 8
  store i64 %.sroa.9.1, ptr %.sroa.2.0..sroa_idx, align 1
  %.sroa.3.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 16
  store i64 %.sroa.1524.2, ptr %.sroa.3.0..sroa_idx, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_remove__DictTint64_tTint64_tT_int64_t_r_bool(ptr nocapture writeonly %0, ptr nocapture %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %.val.i = load i64, ptr %2, align 8
  %4 = lshr i64 %.val.i, 33
  %5 = xor i64 %4, %.val.i
  %6 = mul i64 %5, 1099511628211
  %7 = lshr i64 %6, 33
  %8 = xor i64 %7, %6
  %9 = mul i64 %8, 16777619
  %10 = lshr i64 %9, 33
  %.masked.i.i.i = and i64 %9, 9223372036854775807
  %11 = xor i64 %.masked.i.i.i, %10
  %12 = getelementptr i8, ptr %1, i64 16
  %13 = load i64, ptr %12, align 8
  %.not40 = icmp sgt i64 %13, 0
  br i1 %.not40, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %3
  %14 = load ptr, ptr %1, align 8
  br label %15

15:                                               ; preds = %.lr.ph, %31
  %.pn = phi i64 [ %11, %.lr.ph ], [ %32, %31 ]
  %.01942 = phi i64 [ 0, %.lr.ph ], [ %16, %31 ]
  %.043 = srem i64 %.pn, %13
  %16 = add nuw nsw i64 %.01942, 1
  %17 = getelementptr %Entry, ptr %14, i64 %.043
  %18 = load i8, ptr %17, align 1
  %19 = icmp eq i8 %18, 0
  br i1 %19, label %common.ret, label %20

20:                                               ; preds = %15
  %21 = getelementptr i8, ptr %17, i64 8
  %22 = load i64, ptr %21, align 8
  %23 = icmp eq i64 %22, %11
  br i1 %23, label %24, label %.thread

24:                                               ; preds = %20
  %25 = getelementptr i8, ptr %17, i64 16
  %.val.i25 = load i64, ptr %25, align 8
  %.not33 = icmp eq i64 %.val.i25, %.val.i
  br i1 %.not33, label %33, label %.thread

.thread:                                          ; preds = %20, %24
  %26 = add i64 %.043, %13
  %27 = srem i64 %22, %13
  %28 = sub i64 %26, %27
  %29 = srem i64 %28, %13
  %30 = icmp slt i64 %29, %.01942
  br i1 %30, label %common.ret, label %31

31:                                               ; preds = %.thread
  %32 = add i64 %.043, 1
  %.not = icmp slt i64 %16, %13
  br i1 %.not, label %15, label %._crit_edge

33:                                               ; preds = %24
  %34 = getelementptr i8, ptr %1, i64 8
  %35 = load i64, ptr %34, align 8
  %36 = add i64 %35, -1
  store i64 %36, ptr %34, align 8
  %37 = add i64 %.043, 1
  %38 = srem i64 %37, %13
  %39 = getelementptr %Entry, ptr %14, i64 %38
  %40 = load i8, ptr %39, align 1
  %41 = icmp eq i8 %40, 0
  br i1 %41, label %._crit_edge48, label %.lr.ph47

.lr.ph47:                                         ; preds = %33, %51
  %42 = phi i64 [ %57, %51 ], [ %13, %33 ]
  %.pn50 = phi ptr [ %60, %51 ], [ %39, %33 ]
  %43 = phi i8 [ %61, %51 ], [ %40, %33 ]
  %44 = phi ptr [ %59, %51 ], [ %14, %33 ]
  %.02245 = phi i64 [ %58, %51 ], [ %38, %33 ]
  %.02344 = phi i64 [ %.02245, %51 ], [ %.043, %33 ]
  %.in53 = getelementptr i8, ptr %.pn50, i64 8
  %45 = load i64, ptr %.in53, align 8
  %46 = add i64 %42, %.02245
  %47 = srem i64 %45, %42
  %48 = sub i64 %46, %47
  %49 = srem i64 %48, %42
  %50 = icmp eq i64 %49, 0
  br i1 %50, label %63, label %51

51:                                               ; preds = %.lr.ph47
  %.in51 = getelementptr i8, ptr %.pn50, i64 16
  %52 = getelementptr %Entry, ptr %44, i64 %.02344
  %53 = getelementptr i8, ptr %52, i64 8
  %54 = getelementptr i8, ptr %52, i64 16
  %55 = load <2 x i64>, ptr %.in51, align 8
  store i8 %43, ptr %52, align 1
  store i64 %45, ptr %53, align 8
  store <2 x i64> %55, ptr %54, align 8
  %56 = add i64 %.02245, 1
  %57 = load i64, ptr %12, align 8
  %58 = srem i64 %56, %57
  %59 = load ptr, ptr %1, align 8
  %60 = getelementptr %Entry, ptr %59, i64 %58
  %61 = load i8, ptr %60, align 1
  %62 = icmp eq i8 %61, 0
  br i1 %62, label %._crit_edge48, label %.lr.ph47

63:                                               ; preds = %.lr.ph47
  %64 = getelementptr %Entry, ptr %44, i64 %.02344
  br label %common.ret.sink.split

._crit_edge48:                                    ; preds = %51, %33
  %.023.lcssa = phi i64 [ %.043, %33 ], [ %.02245, %51 ]
  %.lcssa = phi ptr [ %14, %33 ], [ %59, %51 ]
  %65 = getelementptr %Entry, ptr %.lcssa, i64 %.023.lcssa
  br label %common.ret.sink.split

common.ret.sink.split:                            ; preds = %._crit_edge48, %63
  %.sink = phi ptr [ %64, %63 ], [ %65, %._crit_edge48 ]
  store i8 0, ptr %.sink, align 1
  br label %common.ret

common.ret:                                       ; preds = %.thread, %15, %common.ret.sink.split
  %storemerge = phi i8 [ 1, %common.ret.sink.split ], [ 0, %15 ], [ 0, %.thread ]
  store i8 %storemerge, ptr %0, align 1
  ret void

._crit_edge:                                      ; preds = %31, %3
  %66 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_2)
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
  %.val.i = load i64, ptr %2, align 8
  %8 = lshr i64 %.val.i, 33
  %9 = xor i64 %8, %.val.i
  %10 = mul i64 %9, 1099511628211
  %11 = lshr i64 %10, 33
  %12 = xor i64 %11, %10
  %13 = mul i64 %12, 16777619
  %14 = lshr i64 %13, 33
  %.masked.i.i.i = and i64 %13, 9223372036854775807
  %15 = xor i64 %.masked.i.i.i, %14
  %16 = getelementptr i8, ptr %1, i64 16
  %17 = load i64, ptr %16, align 8
  %.not23 = icmp sgt i64 %17, 0
  br i1 %.not23, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %7
  %18 = load ptr, ptr %1, align 8
  br label %20

._crit_edge:                                      ; preds = %36, %7
  %19 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_3)
  tail call void @llvm.trap()
  unreachable

20:                                               ; preds = %.lr.ph, %36
  %.pn = phi i64 [ %15, %.lr.ph ], [ %37, %36 ]
  %.01125 = phi i64 [ 0, %.lr.ph ], [ %21, %36 ]
  %.026 = srem i64 %.pn, %17
  %21 = add nuw nsw i64 %.01125, 1
  %22 = getelementptr %Entry, ptr %18, i64 %.026
  %23 = load i8, ptr %22, align 1
  %24 = icmp eq i8 %23, 0
  br i1 %24, label %common.ret, label %25

25:                                               ; preds = %20
  %26 = getelementptr i8, ptr %22, i64 8
  %27 = load i64, ptr %26, align 8
  %28 = icmp eq i64 %27, %15
  br i1 %28, label %29, label %.thread

29:                                               ; preds = %25
  %30 = getelementptr i8, ptr %22, i64 16
  %.val.i16 = load i64, ptr %30, align 8
  %.not21 = icmp eq i64 %.val.i16, %.val.i
  br i1 %.not21, label %common.ret, label %.thread

.thread:                                          ; preds = %25, %29
  %31 = add i64 %.026, %17
  %32 = srem i64 %27, %17
  %33 = sub i64 %31, %32
  %34 = srem i64 %33, %17
  %35 = icmp slt i64 %34, %.01125
  br i1 %35, label %common.ret, label %36

36:                                               ; preds = %.thread
  %37 = add i64 %.026, 1
  %.not = icmp slt i64 %21, %17
  br i1 %.not, label %20, label %._crit_edge

common.ret:                                       ; preds = %.thread, %20, %29, %3
  %storemerge = phi i8 [ 0, %3 ], [ 1, %29 ], [ 0, %.thread ], [ 0, %20 ]
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
  %8 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_4)
  tail call void @llvm.trap()
  unreachable

9:                                                ; preds = %3
  %.val.i = load i64, ptr %2, align 8
  %10 = lshr i64 %.val.i, 33
  %11 = xor i64 %10, %.val.i
  %12 = mul i64 %11, 1099511628211
  %13 = lshr i64 %12, 33
  %14 = xor i64 %13, %12
  %15 = mul i64 %14, 16777619
  %16 = lshr i64 %15, 33
  %.masked.i.i.i = and i64 %15, 9223372036854775807
  %17 = xor i64 %.masked.i.i.i, %16
  %18 = getelementptr i8, ptr %1, i64 16
  %19 = load i64, ptr %18, align 8
  %.not22 = icmp sgt i64 %19, 0
  br i1 %.not22, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %9
  %20 = load ptr, ptr %1, align 8
  br label %22

._crit_edge:                                      ; preds = %40, %9
  %21 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_5)
  tail call void @llvm.trap()
  unreachable

22:                                               ; preds = %.lr.ph, %40
  %.pn = phi i64 [ %17, %.lr.ph ], [ %41, %40 ]
  %.0924 = phi i64 [ 0, %.lr.ph ], [ %23, %40 ]
  %.025 = srem i64 %.pn, %19
  %23 = add nuw nsw i64 %.0924, 1
  %24 = getelementptr %Entry, ptr %20, i64 %.025
  %25 = load i8, ptr %24, align 1
  %26 = icmp eq i8 %25, 0
  br i1 %26, label %45, label %27

27:                                               ; preds = %22
  %28 = getelementptr i8, ptr %24, i64 8
  %29 = load i64, ptr %28, align 8
  %30 = icmp eq i64 %29, %17
  br i1 %30, label %31, label %.thread

31:                                               ; preds = %27
  %32 = getelementptr i8, ptr %24, i64 16
  %.val.i13 = load i64, ptr %32, align 8
  %.not18 = icmp eq i64 %.val.i13, %.val.i
  br i1 %.not18, label %42, label %.thread

.thread:                                          ; preds = %27, %31
  %33 = add i64 %.025, %19
  %34 = srem i64 %29, %19
  %35 = sub i64 %33, %34
  %36 = srem i64 %35, %19
  %37 = icmp slt i64 %36, %.0924
  br i1 %37, label %38, label %40

38:                                               ; preds = %.thread
  %39 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_6)
  tail call void @llvm.trap()
  unreachable

40:                                               ; preds = %.thread
  %41 = add i64 %.025, 1
  %.not = icmp slt i64 %23, %19
  br i1 %.not, label %22, label %._crit_edge

42:                                               ; preds = %31
  %43 = getelementptr i8, ptr %24, i64 24
  %44 = load i64, ptr %43, align 8
  store i64 %44, ptr %0, align 1
  ret void

45:                                               ; preds = %22
  %46 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_7)
  tail call void @llvm.trap()
  unreachable
}

; Function Attrs: nounwind
define internal fastcc void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr nocapture %0, ptr nocapture readonly %1, i64 %.0.val, i64 %.0.val1) unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 16
  %4 = load i64, ptr %3, align 8
  %.not26 = icmp sgt i64 %4, 0
  br i1 %.not26, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %2
  %5 = lshr i64 %.0.val, 33
  %6 = xor i64 %5, %.0.val
  %7 = mul i64 %6, 1099511628211
  %8 = lshr i64 %7, 33
  %9 = xor i64 %8, %7
  %10 = mul i64 %9, 16777619
  %.masked.i.i.i = and i64 %10, 9223372036854775807
  %11 = lshr i64 %10, 33
  %12 = xor i64 %.masked.i.i.i, %11
  %13 = urem i64 %12, %4
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %44
  %14 = phi i64 [ %45, %44 ], [ %4, %.lr.ph.preheader ]
  %.032 = phi i64 [ %48, %44 ], [ %13, %.lr.ph.preheader ]
  %.02631 = phi i64 [ %46, %44 ], [ 0, %.lr.ph.preheader ]
  %.02730 = phi i64 [ %15, %44 ], [ 0, %.lr.ph.preheader ]
  %.02829 = phi i64 [ %.129, %44 ], [ %.0.val1, %.lr.ph.preheader ]
  %.03028 = phi i64 [ %.131, %44 ], [ %12, %.lr.ph.preheader ]
  %.0927 = phi i64 [ %.110, %44 ], [ %.0.val, %.lr.ph.preheader ]
  %15 = add nuw nsw i64 %.02730, 1
  %16 = load ptr, ptr %1, align 8
  %17 = getelementptr %Entry, ptr %16, i64 %.032
  %18 = load i8, ptr %17, align 1
  %19 = icmp eq i8 %18, 0
  %20 = getelementptr i8, ptr %17, i64 8
  br i1 %19, label %60, label %21

21:                                               ; preds = %.lr.ph
  %22 = load i64, ptr %20, align 8
  %23 = icmp eq i64 %22, %.03028
  br i1 %23, label %24, label %.thread

24:                                               ; preds = %21
  %25 = getelementptr i8, ptr %17, i64 16
  %.val.i35 = load i64, ptr %25, align 8
  %.not13 = icmp eq i64 %.val.i35, %.0927
  br i1 %.not13, label %49, label %.thread

.thread:                                          ; preds = %21, %24
  %26 = add i64 %14, %.032
  %27 = srem i64 %22, %14
  %28 = sub i64 %26, %27
  %29 = srem i64 %28, %14
  %30 = icmp slt i64 %29, %.02631
  br i1 %30, label %31, label %44

31:                                               ; preds = %.thread
  %32 = getelementptr i8, ptr %17, i64 16
  %33 = load i64, ptr %32, align 8
  %34 = getelementptr i8, ptr %17, i64 24
  %35 = load i64, ptr %34, align 8
  store i64 %.03028, ptr %20, align 8
  store i64 %.0927, ptr %32, align 8
  store i64 %.02829, ptr %34, align 8
  %36 = load ptr, ptr %1, align 8
  %37 = getelementptr %Entry, ptr %36, i64 %.032
  store i8 %18, ptr %37, align 1
  %38 = getelementptr i8, ptr %37, i64 8
  %39 = load i64, ptr %20, align 8
  store i64 %39, ptr %38, align 8
  %40 = getelementptr i8, ptr %37, i64 16
  %41 = load i64, ptr %32, align 8
  store i64 %41, ptr %40, align 8
  %42 = getelementptr i8, ptr %37, i64 24
  %43 = load i64, ptr %34, align 8
  store i64 %43, ptr %42, align 8
  %.pre = load i64, ptr %3, align 8
  br label %44

44:                                               ; preds = %.thread, %31
  %45 = phi i64 [ %.pre, %31 ], [ %14, %.thread ]
  %.110 = phi i64 [ %33, %31 ], [ %.0927, %.thread ]
  %.131 = phi i64 [ %22, %31 ], [ %.03028, %.thread ]
  %.129 = phi i64 [ %35, %31 ], [ %.02829, %.thread ]
  %.1 = phi i64 [ %29, %31 ], [ %.02631, %.thread ]
  %46 = add nsw i64 %.1, 1
  %47 = add i64 %.032, 1
  %48 = srem i64 %47, %45
  %.not = icmp slt i64 %15, %45
  br i1 %.not, label %.lr.ph, label %._crit_edge

common.ret:                                       ; preds = %60, %49
  ret void

49:                                               ; preds = %24
  %50 = getelementptr i8, ptr %17, i64 16
  %51 = getelementptr i8, ptr %17, i64 24
  store i64 %.02829, ptr %51, align 8
  %52 = load ptr, ptr %1, align 8
  %53 = getelementptr %Entry, ptr %52, i64 %.032
  store i8 %18, ptr %53, align 1
  %54 = getelementptr i8, ptr %53, i64 8
  %55 = load i64, ptr %20, align 8
  store i64 %55, ptr %54, align 8
  %56 = getelementptr i8, ptr %53, i64 16
  %57 = load i64, ptr %50, align 8
  store i64 %57, ptr %56, align 8
  %58 = getelementptr i8, ptr %53, i64 24
  %59 = load i64, ptr %51, align 8
  store i64 %59, ptr %58, align 8
  br label %common.ret

60:                                               ; preds = %.lr.ph
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %20, i8 0, i64 24, i1 false)
  %61 = load ptr, ptr %1, align 8
  %62 = getelementptr %Entry, ptr %61, i64 %.032
  %63 = getelementptr i8, ptr %62, i64 8
  %64 = getelementptr i8, ptr %62, i64 16
  %65 = getelementptr i8, ptr %62, i64 24
  store i8 1, ptr %62, align 1
  store i64 %.03028, ptr %63, align 8
  store i64 %.0927, ptr %64, align 8
  store i64 %.02829, ptr %65, align 8
  %66 = getelementptr i8, ptr %0, i64 8
  %67 = load i64, ptr %66, align 8
  %68 = add i64 %67, 1
  store i64 %68, ptr %66, align 8
  br label %common.ret

._crit_edge:                                      ; preds = %44, %2
  %69 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
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
  br i1 %24, label %.lr.ph.i, label %.preheader19.i

.preheader19.i:                                   ; preds = %.lr.ph.i, %rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i
  %25 = icmp sgt i64 %10, 0
  br i1 %25, label %.lr.ph22.i, label %rl_m__grow__DictTint64_tTint64_tT.exit

.lr.ph.i:                                         ; preds = %rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i, %.lr.ph.i
  %.020.i = phi i64 [ %28, %.lr.ph.i ], [ 0, %rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i ]
  %26 = load ptr, ptr %1, align 8
  %27 = getelementptr %Entry, ptr %26, i64 %.020.i
  store i8 0, ptr %27, align 1
  %28 = add nuw nsw i64 %.020.i, 1
  %29 = load i64, ptr %9, align 8
  %30 = icmp slt i64 %28, %29
  br i1 %30, label %.lr.ph.i, label %.preheader19.i

.lr.ph22.i:                                       ; preds = %.preheader19.i, %36
  %.121.i = phi i64 [ %37, %36 ], [ 0, %.preheader19.i ]
  %31 = getelementptr %Entry, ptr %17, i64 %.121.i
  %32 = load i8, ptr %31, align 1
  %.not.i = icmp eq i8 %32, 0
  br i1 %.not.i, label %36, label %33

33:                                               ; preds = %.lr.ph22.i
  %34 = getelementptr i8, ptr %31, i64 16
  %35 = getelementptr i8, ptr %31, i64 24
  %.val16.i = load i64, ptr %34, align 8
  %.val17.i = load i64, ptr %35, align 8
  tail call fastcc void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr nonnull %1, ptr nonnull %1, i64 %.val16.i, i64 %.val17.i)
  br label %36

36:                                               ; preds = %33, %.lr.ph22.i
  %37 = add nuw nsw i64 %.121.i, 1
  %38 = icmp slt i64 %37, %10
  br i1 %38, label %.lr.ph22.i, label %rl_m__grow__DictTint64_tTint64_tT.exit

rl_m__grow__DictTint64_tTint64_tT.exit:           ; preds = %36, %.preheader19.i
  tail call void @free(ptr %17)
  br label %39

39:                                               ; preds = %4, %rl_m__grow__DictTint64_tTint64_tT.exit
  %.val = load i64, ptr %2, align 8
  %.val1 = load i64, ptr %3, align 8
  tail call fastcc void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr %1, ptr %1, i64 %.val, i64 %.val1)
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
  %.03 = phi i64 [ %7, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %5 = load ptr, ptr %0, align 8
  %6 = getelementptr %Entry, ptr %5, i64 %.03
  store i8 0, ptr %6, align 1
  %7 = add nuw nsw i64 %.03, 1
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
  %.04 = phi i64 [ %10, %.lr.ph ], [ %6, %2 ]
  %8 = load ptr, ptr %0, align 8
  %9 = getelementptr i8, ptr %8, i64 %.04
  store i8 0, ptr %9, align 1
  %10 = add nsw i64 %.04, 1
  %11 = load i64, ptr %3, align 8
  %12 = icmp slt i64 %10, %11
  br i1 %12, label %.lr.ph, label %._crit_edge.loopexit

._crit_edge.loopexit:                             ; preds = %.lr.ph
  %.pre = load i64, ptr %1, align 8
  %.pre6 = sub i64 %11, %.pre
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %2
  %.pre-phi = phi i64 [ %.pre6, %._crit_edge.loopexit ], [ %6, %2 ]
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
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
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
  br i1 %13, label %.lr.ph.preheader.i, label %.preheader12.i

.lr.ph.preheader.i:                               ; preds = %9
  tail call void @llvm.memset.p0.i64(ptr align 1 %11, i8 0, i64 %10, i1 false)
  br label %.preheader12.i

.preheader12.i:                                   ; preds = %.lr.ph.preheader.i, %9
  %14 = icmp sgt i64 %4, 0
  %.pre.i = load ptr, ptr %0, align 8
  br i1 %14, label %iter.check, label %.preheader.i

iter.check:                                       ; preds = %.preheader12.i
  %.pre.i3 = ptrtoint ptr %.pre.i to i64
  %min.iters.check = icmp ult i64 %4, 8
  %15 = sub i64 %12, %.pre.i3
  %diff.check = icmp ult i64 %15, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check4 = icmp ult i64 %4, 32
  br i1 %min.iters.check4, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %4, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %16 = getelementptr i8, ptr %11, i64 %index
  %17 = getelementptr i8, ptr %.pre.i, i64 %index
  %18 = getelementptr i8, ptr %17, i64 16
  %wide.load = load <16 x i8>, ptr %17, align 1
  %wide.load5 = load <16 x i8>, ptr %18, align 1
  %19 = getelementptr i8, ptr %16, i64 16
  store <16 x i8> %wide.load, ptr %16, align 1
  store <16 x i8> %wide.load5, ptr %19, align 1
  %index.next = add nuw i64 %index, 32
  %20 = icmp eq i64 %index.next, %n.vec
  br i1 %20, label %middle.block, label %vector.body, !llvm.loop !11

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %4, %n.vec
  br i1 %cmp.n, label %.preheader.i, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %4, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.preheader, label %vec.epilog.ph

.lr.ph15.i.preheader:                             ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec7, %vec.epilog.middle.block ]
  br label %.lr.ph15.i

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec7 = and i64 %4, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index8 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next10, %vec.epilog.vector.body ]
  %21 = getelementptr i8, ptr %11, i64 %index8
  %22 = getelementptr i8, ptr %.pre.i, i64 %index8
  %wide.load9 = load <8 x i8>, ptr %22, align 1
  store <8 x i8> %wide.load9, ptr %21, align 1
  %index.next10 = add nuw i64 %index8, 8
  %23 = icmp eq i64 %index.next10, %n.vec7
  br i1 %23, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !12

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n11 = icmp eq i64 %4, %n.vec7
  br i1 %cmp.n11, label %.preheader.i, label %.lr.ph15.i.preheader

.preheader.i:                                     ; preds = %.lr.ph15.i, %middle.block, %vec.epilog.middle.block, %.preheader12.i
  tail call void @free(ptr %.pre.i)
  store i64 %10, ptr %6, align 8
  store ptr %11, ptr %0, align 8
  %.pre = load i64, ptr %3, align 8
  br label %rl_m__grow__VectorTint8_tT_int64_t.exit

.lr.ph15.i:                                       ; preds = %.lr.ph15.i.preheader, %.lr.ph15.i
  %.114.i = phi i64 [ %27, %.lr.ph15.i ], [ %.114.i.ph, %.lr.ph15.i.preheader ]
  %24 = getelementptr i8, ptr %11, i64 %.114.i
  %25 = getelementptr i8, ptr %.pre.i, i64 %.114.i
  %26 = load i8, ptr %25, align 1
  store i8 %26, ptr %24, align 1
  %27 = add nuw nsw i64 %.114.i, 1
  %28 = icmp slt i64 %27, %4
  br i1 %28, label %.lr.ph15.i, label %.preheader.i, !llvm.loop !13

rl_m__grow__VectorTint8_tT_int64_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge, %.preheader.i
  %29 = phi ptr [ %.pre2, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge ], [ %11, %.preheader.i ]
  %30 = phi i64 [ %4, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge ], [ %.pre, %.preheader.i ]
  %31 = getelementptr i8, ptr %29, i64 %30
  %32 = load i8, ptr %1, align 1
  store i8 %32, ptr %31, align 1
  %33 = load i64, ptr %3, align 8
  %34 = add i64 %33, 1
  store i64 %34, ptr %3, align 8
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
  %12 = ptrtoint ptr %11 to i64
  %13 = trunc i64 %5 to i63
  %14 = icmp sgt i63 %13, 0
  br i1 %14, label %.lr.ph.preheader.i, label %.preheader12.i

.lr.ph.preheader.i:                               ; preds = %9
  tail call void @llvm.memset.p0.i64(ptr align 8 %11, i8 0, i64 %10, i1 false)
  br label %.preheader12.i

.preheader12.i:                                   ; preds = %.lr.ph.preheader.i, %9
  %15 = icmp sgt i64 %4, 0
  %.pre.i = load ptr, ptr %0, align 8
  br i1 %15, label %.lr.ph15.i.preheader, label %.preheader.i

.lr.ph15.i.preheader:                             ; preds = %.preheader12.i
  %.pre.i3 = ptrtoint ptr %.pre.i to i64
  %min.iters.check = icmp ult i64 %4, 6
  %16 = sub i64 %12, %.pre.i3
  %diff.check = icmp ult i64 %16, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.preheader5, label %vector.ph

.lr.ph15.i.preheader5:                            ; preds = %middle.block, %.lr.ph15.i.preheader
  %.114.i.ph = phi i64 [ 0, %.lr.ph15.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph15.i

vector.ph:                                        ; preds = %.lr.ph15.i.preheader
  %n.vec = and i64 %4, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %17 = getelementptr i64, ptr %11, i64 %index
  %18 = getelementptr i64, ptr %.pre.i, i64 %index
  %19 = getelementptr i8, ptr %18, i64 16
  %wide.load = load <2 x i64>, ptr %18, align 8
  %wide.load4 = load <2 x i64>, ptr %19, align 8
  %20 = getelementptr i8, ptr %17, i64 16
  store <2 x i64> %wide.load, ptr %17, align 8
  store <2 x i64> %wide.load4, ptr %20, align 8
  %index.next = add nuw i64 %index, 4
  %21 = icmp eq i64 %index.next, %n.vec
  br i1 %21, label %middle.block, label %vector.body, !llvm.loop !14

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %4, %n.vec
  br i1 %cmp.n, label %.preheader.i, label %.lr.ph15.i.preheader5

.preheader.i:                                     ; preds = %.lr.ph15.i, %middle.block, %.preheader12.i
  tail call void @free(ptr %.pre.i)
  %22 = shl i64 %5, 1
  store i64 %22, ptr %6, align 8
  store ptr %11, ptr %0, align 8
  %.pre = load i64, ptr %3, align 8
  br label %rl_m__grow__VectorTint64_tT_int64_t.exit

.lr.ph15.i:                                       ; preds = %.lr.ph15.i.preheader5, %.lr.ph15.i
  %.114.i = phi i64 [ %26, %.lr.ph15.i ], [ %.114.i.ph, %.lr.ph15.i.preheader5 ]
  %23 = getelementptr i64, ptr %11, i64 %.114.i
  %24 = getelementptr i64, ptr %.pre.i, i64 %.114.i
  %25 = load i64, ptr %24, align 8
  store i64 %25, ptr %23, align 8
  %26 = add nuw nsw i64 %.114.i, 1
  %27 = icmp slt i64 %26, %4
  br i1 %27, label %.lr.ph15.i, label %.preheader.i, !llvm.loop !15

rl_m__grow__VectorTint64_tT_int64_t.exit:         ; preds = %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge, %.preheader.i
  %28 = phi ptr [ %.pre2, %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge ], [ %11, %.preheader.i ]
  %29 = phi i64 [ %4, %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge ], [ %.pre, %.preheader.i ]
  %30 = getelementptr i64, ptr %28, i64 %29
  %31 = load i64, ptr %1, align 8
  store i64 %31, ptr %30, align 8
  %32 = load i64, ptr %3, align 8
  %33 = add i64 %32, 1
  store i64 %33, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = load i64, ptr %2, align 8
  %5 = icmp sgt i64 %4, -1
  br i1 %5, label %8, label %6

6:                                                ; preds = %3
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

8:                                                ; preds = %3
  %9 = getelementptr i8, ptr %1, i64 8
  %10 = load i64, ptr %9, align 8
  %11 = icmp slt i64 %4, %10
  br i1 %11, label %14, label %12

12:                                               ; preds = %8
  %13 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

8:                                                ; preds = %3
  %9 = getelementptr i8, ptr %1, i64 8
  %10 = load i64, ptr %9, align 8
  %11 = icmp slt i64 %4, %10
  br i1 %11, label %14, label %12

12:                                               ; preds = %8
  %13 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
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
  %.not3.i = icmp eq i64 %4, 0
  br i1 %.not3.i, label %rl_m_drop__VectorTint8_tT.exit, label %5

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
  %.03.i = phi i64 [ %11, %.lr.ph.i ], [ 0, %rl_m_drop__VectorTint8_tT.exit ]
  %9 = load ptr, ptr %0, align 8
  %10 = getelementptr i8, ptr %9, i64 %.03.i
  store i8 0, ptr %10, align 1
  %11 = add nuw nsw i64 %.03.i, 1
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
  %17 = phi i64 [ %48, %rl_m_append__VectorTint8_tT_int8_t.exit ], [ %.pr, %.lr.ph.preheader ]
  %storemerge2 = phi i64 [ %49, %rl_m_append__VectorTint8_tT_int8_t.exit ], [ 0, %.lr.ph.preheader ]
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
  br i1 %27, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %23
  tail call void @llvm.memset.p0.i64(ptr align 1 %25, i8 0, i64 %24, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %23
  %28 = icmp sgt i64 %17, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %28, label %iter.check, label %.preheader.i.i

iter.check:                                       ; preds = %.preheader12.i.i
  %.pre.i.i3 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %17, 8
  %29 = sub i64 %26, %.pre.i.i3
  %diff.check = icmp ult i64 %29, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check4 = icmp ult i64 %17, 32
  br i1 %min.iters.check4, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %17, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %30 = getelementptr i8, ptr %25, i64 %index
  %31 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %32 = getelementptr i8, ptr %31, i64 16
  %wide.load = load <16 x i8>, ptr %31, align 1
  %wide.load5 = load <16 x i8>, ptr %32, align 1
  %33 = getelementptr i8, ptr %30, i64 16
  store <16 x i8> %wide.load, ptr %30, align 1
  store <16 x i8> %wide.load5, ptr %33, align 1
  %index.next = add nuw i64 %index, 32
  %34 = icmp eq i64 %index.next, %n.vec
  br i1 %34, label %middle.block, label %vector.body, !llvm.loop !16

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %17, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %17, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.preheader, label %vec.epilog.ph

.lr.ph15.i.i.preheader:                           ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec7, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec7 = and i64 %17, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index8 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next10, %vec.epilog.vector.body ]
  %35 = getelementptr i8, ptr %25, i64 %index8
  %36 = getelementptr i8, ptr %.pre.i.i, i64 %index8
  %wide.load9 = load <8 x i8>, ptr %36, align 1
  store <8 x i8> %wide.load9, ptr %35, align 1
  %index.next10 = add nuw i64 %index8, 8
  %37 = icmp eq i64 %index.next10, %n.vec7
  br i1 %37, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !17

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n11 = icmp eq i64 %17, %n.vec7
  br i1 %cmp.n11, label %.preheader.i.i, label %.lr.ph15.i.i.preheader

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %24, ptr %3, align 8
  store ptr %25, ptr %0, align 8
  %.pre.i = load i64, ptr %7, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %41, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader ]
  %38 = getelementptr i8, ptr %25, i64 %.114.i.i
  %39 = getelementptr i8, ptr %.pre.i.i, i64 %.114.i.i
  %40 = load i8, ptr %39, align 1
  store i8 %40, ptr %38, align 1
  %41 = add nuw nsw i64 %.114.i.i, 1
  %42 = icmp slt i64 %41, %17
  br i1 %42, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !18

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %43 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %25, %.preheader.i.i ]
  %44 = phi i64 [ %17, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %45 = getelementptr i8, ptr %43, i64 %44
  %46 = load i8, ptr %19, align 1
  store i8 %46, ptr %45, align 1
  %47 = load i64, ptr %7, align 8
  %48 = add i64 %47, 1
  store i64 %48, ptr %7, align 8
  %49 = add nuw nsw i64 %storemerge2, 1
  %50 = load i64, ptr %14, align 8
  %51 = icmp slt i64 %49, %50
  br i1 %51, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit, label %rl_m_init__VectorTint8_tT.exit._crit_edge

rl_m_init__VectorTint8_tT.exit._crit_edge:        ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit, %rl_m_init__VectorTint8_tT.exit.preheader
  ret void
}

; Function Attrs: nounwind
define void @rl_m_assign__VectorTint64_tT_VectorTint64_tT(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 16
  %4 = load i64, ptr %3, align 8
  %.not3.i = icmp eq i64 %4, 0
  br i1 %.not3.i, label %rl_m_drop__VectorTint64_tT.exit, label %5

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
  %.03.i = phi i64 [ %11, %.lr.ph.i ], [ 0, %rl_m_drop__VectorTint64_tT.exit ]
  %9 = load ptr, ptr %0, align 8
  %10 = getelementptr i64, ptr %9, i64 %.03.i
  store i64 0, ptr %10, align 8
  %11 = add nuw nsw i64 %.03.i, 1
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
  %17 = phi i64 [ %47, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %.pr, %.lr.ph.preheader ]
  %storemerge2 = phi i64 [ %48, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ 0, %.lr.ph.preheader ]
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
  %26 = ptrtoint ptr %25 to i64
  %27 = trunc i64 %20 to i63
  %28 = icmp sgt i63 %27, 0
  br i1 %28, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %23
  tail call void @llvm.memset.p0.i64(ptr align 8 %25, i8 0, i64 %24, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %23
  %29 = icmp sgt i64 %17, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %29, label %.lr.ph15.i.i.preheader, label %.preheader.i.i

.lr.ph15.i.i.preheader:                           ; preds = %.preheader12.i.i
  %.pre.i.i3 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %17, 4
  %30 = sub i64 %26, %.pre.i.i3
  %diff.check = icmp ult i64 %30, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader5, label %vector.ph

.lr.ph15.i.i.preheader5:                          ; preds = %middle.block, %.lr.ph15.i.i.preheader
  %.114.i.i.ph = phi i64 [ 0, %.lr.ph15.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph15.i.i

vector.ph:                                        ; preds = %.lr.ph15.i.i.preheader
  %n.vec = and i64 %17, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %31 = getelementptr i64, ptr %25, i64 %index
  %32 = getelementptr i64, ptr %.pre.i.i, i64 %index
  %33 = getelementptr i8, ptr %32, i64 16
  %wide.load = load <2 x i64>, ptr %32, align 8
  %wide.load4 = load <2 x i64>, ptr %33, align 8
  %34 = getelementptr i8, ptr %31, i64 16
  store <2 x i64> %wide.load, ptr %31, align 8
  store <2 x i64> %wide.load4, ptr %34, align 8
  %index.next = add nuw i64 %index, 4
  %35 = icmp eq i64 %index.next, %n.vec
  br i1 %35, label %middle.block, label %vector.body, !llvm.loop !19

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %17, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph15.i.i.preheader5

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %.preheader12.i.i
  tail call void @free(ptr %.pre.i.i)
  %36 = shl i64 %20, 1
  store i64 %36, ptr %3, align 8
  store ptr %25, ptr %0, align 8
  %.pre.i = load i64, ptr %7, align 8
  br label %rl_m_append__VectorTint64_tT_int64_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader5, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %40, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader5 ]
  %37 = getelementptr i64, ptr %25, i64 %.114.i.i
  %38 = getelementptr i64, ptr %.pre.i.i, i64 %.114.i.i
  %39 = load i64, ptr %38, align 8
  store i64 %39, ptr %37, align 8
  %40 = add nuw nsw i64 %.114.i.i, 1
  %41 = icmp slt i64 %40, %17
  br i1 %41, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !20

rl_m_append__VectorTint64_tT_int64_t.exit:        ; preds = %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %42 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge.i ], [ %25, %.preheader.i.i ]
  %43 = phi i64 [ %17, %.rl_m__grow__VectorTint64_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %44 = getelementptr i64, ptr %42, i64 %43
  %45 = load i64, ptr %19, align 8
  store i64 %45, ptr %44, align 8
  %46 = load i64, ptr %7, align 8
  %47 = add i64 %46, 1
  store i64 %47, ptr %7, align 8
  %48 = add nuw nsw i64 %storemerge2, 1
  %49 = load i64, ptr %14, align 8
  %50 = icmp slt i64 %48, %49
  br i1 %50, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit, label %rl_m_init__VectorTint64_tT.exit._crit_edge

rl_m_init__VectorTint64_tT.exit._crit_edge:       ; preds = %rl_m_append__VectorTint64_tT_int64_t.exit, %rl_m_init__VectorTint64_tT.exit.preheader
  ret void
}

; Function Attrs: mustprogress nounwind willreturn
define void @rl_m_drop__VectorTint8_tT(ptr nocapture %0) local_unnamed_addr #6 {
  %2 = getelementptr i8, ptr %0, i64 16
  %3 = load i64, ptr %2, align 8
  %.not3 = icmp eq i64 %3, 0
  br i1 %.not3, label %6, label %4

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
  %.not3 = icmp eq i64 %3, 0
  br i1 %.not3, label %6, label %4

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
  %.03 = phi i64 [ %6, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %4 = load ptr, ptr %0, align 8
  %5 = getelementptr i8, ptr %4, i64 %.03
  store i8 0, ptr %5, align 1
  %6 = add nuw nsw i64 %.03, 1
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
  %.03 = phi i64 [ %6, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %4 = load ptr, ptr %0, align 8
  %5 = getelementptr i64, ptr %4, i64 %.03
  store i64 0, ptr %5, align 8
  %6 = add nuw nsw i64 %.03, 1
  %7 = load i64, ptr %2, align 8
  %8 = icmp slt i64 %6, %7
  br i1 %8, label %.lr.ph, label %._crit_edge

._crit_edge:                                      ; preds = %.lr.ph
  ret void
}

; Function Attrs: nounwind
define void @rl_m_to_indented_lines__String_r_String(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = alloca ptr, align 8
  %4 = alloca %String, align 8
  %5 = getelementptr inbounds i8, ptr %4, i64 8
  store i64 0, ptr %5, align 8
  %6 = getelementptr inbounds i8, ptr %4, i64 16
  store i64 4, ptr %6, align 8
  %7 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %7, ptr %4, align 8
  br label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %.lr.ph.i.i, %2
  %.03.i.i = phi i64 [ %10, %.lr.ph.i.i ], [ 0, %2 ]
  %8 = load ptr, ptr %4, align 8
  %9 = getelementptr i8, ptr %8, i64 %.03.i.i
  store i8 0, ptr %9, align 1
  %10 = add nuw nsw i64 %.03.i.i, 1
  %11 = load i64, ptr %6, align 8
  %12 = icmp slt i64 %10, %11
  br i1 %12, label %.lr.ph.i.i, label %rl_m_init__VectorTint8_tT.exit.i

rl_m_init__VectorTint8_tT.exit.i:                 ; preds = %.lr.ph.i.i
  %13 = load i64, ptr %5, align 8
  %14 = add i64 %13, 1
  %15 = icmp sgt i64 %11, %14
  br i1 %15, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i, label %16

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i: ; preds = %rl_m_init__VectorTint8_tT.exit.i
  %.pre2.i.i = load ptr, ptr %4, align 8
  br label %rl_m_init__String.exit

16:                                               ; preds = %rl_m_init__VectorTint8_tT.exit.i
  %17 = shl i64 %14, 1
  %18 = tail call ptr @malloc(i64 %17)
  %19 = ptrtoint ptr %18 to i64
  %20 = icmp sgt i64 %17, 0
  br i1 %20, label %.lr.ph.preheader.i.i.i, label %.preheader12.i.i.i

.lr.ph.preheader.i.i.i:                           ; preds = %16
  tail call void @llvm.memset.p0.i64(ptr align 1 %18, i8 0, i64 %17, i1 false)
  br label %.preheader12.i.i.i

.preheader12.i.i.i:                               ; preds = %.lr.ph.preheader.i.i.i, %16
  %21 = icmp sgt i64 %13, 0
  %.pre.i.i.i = load ptr, ptr %4, align 8
  br i1 %21, label %iter.check, label %.preheader.i.i.i

iter.check:                                       ; preds = %.preheader12.i.i.i
  %.pre.i.i.i227 = ptrtoint ptr %.pre.i.i.i to i64
  %min.iters.check = icmp ult i64 %13, 8
  %22 = sub i64 %19, %.pre.i.i.i227
  %diff.check = icmp ult i64 %22, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check228 = icmp ult i64 %13, 32
  br i1 %min.iters.check228, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %13, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %23 = getelementptr i8, ptr %18, i64 %index
  %24 = getelementptr i8, ptr %.pre.i.i.i, i64 %index
  %25 = getelementptr i8, ptr %24, i64 16
  %wide.load = load <16 x i8>, ptr %24, align 1
  %wide.load229 = load <16 x i8>, ptr %25, align 1
  %26 = getelementptr i8, ptr %23, i64 16
  store <16 x i8> %wide.load, ptr %23, align 1
  store <16 x i8> %wide.load229, ptr %26, align 1
  %index.next = add nuw i64 %index, 32
  %27 = icmp eq i64 %index.next, %n.vec
  br i1 %27, label %middle.block, label %vector.body, !llvm.loop !21

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %13, %n.vec
  br i1 %cmp.n, label %.preheader.i.i.i, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %13, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.i.preheader, label %vec.epilog.ph

.lr.ph15.i.i.i.preheader:                         ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec231, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i.i

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec231 = and i64 %13, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index232 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next234, %vec.epilog.vector.body ]
  %28 = getelementptr i8, ptr %18, i64 %index232
  %29 = getelementptr i8, ptr %.pre.i.i.i, i64 %index232
  %wide.load233 = load <8 x i8>, ptr %29, align 1
  store <8 x i8> %wide.load233, ptr %28, align 1
  %index.next234 = add nuw i64 %index232, 8
  %30 = icmp eq i64 %index.next234, %n.vec231
  br i1 %30, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !22

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n235 = icmp eq i64 %13, %n.vec231
  br i1 %cmp.n235, label %.preheader.i.i.i, label %.lr.ph15.i.i.i.preheader

.preheader.i.i.i:                                 ; preds = %.lr.ph15.i.i.i, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i.i
  tail call void @free(ptr %.pre.i.i.i)
  store i64 %17, ptr %6, align 8
  store ptr %18, ptr %4, align 8
  br label %rl_m_init__String.exit

.lr.ph15.i.i.i:                                   ; preds = %.lr.ph15.i.i.i.preheader, %.lr.ph15.i.i.i
  %.114.i.i.i = phi i64 [ %34, %.lr.ph15.i.i.i ], [ %.114.i.i.i.ph, %.lr.ph15.i.i.i.preheader ]
  %31 = getelementptr i8, ptr %18, i64 %.114.i.i.i
  %32 = getelementptr i8, ptr %.pre.i.i.i, i64 %.114.i.i.i
  %33 = load i8, ptr %32, align 1
  store i8 %33, ptr %31, align 1
  %34 = add nuw nsw i64 %.114.i.i.i, 1
  %35 = icmp slt i64 %34, %13
  br i1 %35, label %.lr.ph15.i.i.i, label %.preheader.i.i.i, !llvm.loop !23

rl_m_init__String.exit:                           ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i, %.preheader.i.i.i
  %36 = phi ptr [ %.pre2.i.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i ], [ %18, %.preheader.i.i.i ]
  %37 = getelementptr i8, ptr %36, i64 %13
  store i8 0, ptr %37, align 1
  %38 = load i64, ptr %5, align 8
  %39 = add i64 %38, 1
  store i64 %39, ptr %5, align 8
  %40 = getelementptr i8, ptr %1, i64 8
  %41 = load i64, ptr %40, align 8
  %42 = add i64 %41, -1
  %43 = icmp sgt i64 %42, 0
  br i1 %43, label %.lr.ph, label %.lr.ph.i.i95.preheader

.lr.ph.i.i95.preheader.loopexit:                  ; preds = %rl__indent_string__String_int64_t.exit94
  %.pre = load i64, ptr %5, align 8
  br label %.lr.ph.i.i95.preheader

.lr.ph.i.i95.preheader:                           ; preds = %.lr.ph.i.i95.preheader.loopexit, %rl_m_init__String.exit
  %44 = phi i64 [ %.pre, %.lr.ph.i.i95.preheader.loopexit ], [ %39, %rl_m_init__String.exit ]
  %45 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store i32 0, ptr %45, align 1
  %46 = icmp sgt i64 %44, 0
  br i1 %46, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, label %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit

.lr.ph:                                           ; preds = %rl_m_init__String.exit, %rl__indent_string__String_int64_t.exit94
  %47 = phi i64 [ %503, %rl__indent_string__String_int64_t.exit94 ], [ %41, %rl_m_init__String.exit ]
  %.0217 = phi i64 [ %502, %rl__indent_string__String_int64_t.exit94 ], [ 0, %rl_m_init__String.exit ]
  %.0204216 = phi i64 [ %.1205, %rl__indent_string__String_int64_t.exit94 ], [ 0, %rl_m_init__String.exit ]
  %48 = icmp sgt i64 %.0217, -1
  br i1 %48, label %51, label %49

49:                                               ; preds = %.lr.ph
  %50 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

51:                                               ; preds = %.lr.ph
  %52 = icmp slt i64 %.0217, %47
  br i1 %52, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %53

53:                                               ; preds = %51
  %54 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %51
  %55 = load ptr, ptr %1, align 8
  %56 = getelementptr i8, ptr %55, i64 %.0217
  %57 = load i8, ptr %56, align 1
  switch i8 %57, label %rl_m_get__String_int64_t_r_int8_tRef.exit5 [
    i8 40, label %rl_m_get__String_int64_t_r_int8_tRef.exit67
    i8 91, label %rl_m_get__String_int64_t_r_int8_tRef.exit67
    i8 123, label %rl_m_get__String_int64_t_r_int8_tRef.exit67
    i8 41, label %rl_is_close_paren__int8_t_r_bool.exit.thread
    i8 125, label %rl_is_close_paren__int8_t_r_bool.exit.thread
    i8 93, label %rl_is_close_paren__int8_t_r_bool.exit.thread
    i8 44, label %rl_m_get__String_int64_t_r_int8_tRef.exit15
  ]

rl_m_get__String_int64_t_r_int8_tRef.exit5:       ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %58 = load i64, ptr %5, align 8
  %59 = icmp sgt i64 %58, 0
  br i1 %59, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i, label %60

60:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit5
  %61 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i:     ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit5
  %62 = load ptr, ptr %4, align 8
  %63 = getelementptr i8, ptr %62, i64 %58
  %64 = getelementptr i8, ptr %63, i64 -1
  store i8 %57, ptr %64, align 1
  %65 = load i64, ptr %5, align 8
  %66 = add i64 %65, 1
  %67 = load i64, ptr %6, align 8
  %68 = icmp sgt i64 %67, %66
  br i1 %68, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i13, label %69

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i13: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i
  %.pre2.i.i14 = load ptr, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit

69:                                               ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i
  %70 = shl i64 %66, 1
  %71 = tail call ptr @malloc(i64 %70)
  %72 = ptrtoint ptr %71 to i64
  %73 = icmp sgt i64 %70, 0
  br i1 %73, label %.lr.ph.preheader.i.i.i12, label %.preheader12.i.i.i6

.lr.ph.preheader.i.i.i12:                         ; preds = %69
  tail call void @llvm.memset.p0.i64(ptr align 1 %71, i8 0, i64 %70, i1 false)
  br label %.preheader12.i.i.i6

.preheader12.i.i.i6:                              ; preds = %.lr.ph.preheader.i.i.i12, %69
  %74 = icmp sgt i64 %65, 0
  %.pre.i.i.i7 = load ptr, ptr %4, align 8
  br i1 %74, label %iter.check242, label %.preheader.i.i.i8

iter.check242:                                    ; preds = %.preheader12.i.i.i6
  %.pre.i.i.i7237 = ptrtoint ptr %.pre.i.i.i7 to i64
  %min.iters.check240 = icmp ult i64 %65, 8
  %75 = sub i64 %72, %.pre.i.i.i7237
  %diff.check238 = icmp ult i64 %75, 32
  %or.cond698 = select i1 %min.iters.check240, i1 true, i1 %diff.check238
  br i1 %or.cond698, label %.lr.ph15.i.i.i10.preheader, label %vector.main.loop.iter.check244

vector.main.loop.iter.check244:                   ; preds = %iter.check242
  %min.iters.check243 = icmp ult i64 %65, 32
  br i1 %min.iters.check243, label %vec.epilog.ph256, label %vector.ph245

vector.ph245:                                     ; preds = %vector.main.loop.iter.check244
  %n.vec247 = and i64 %65, 9223372036854775776
  br label %vector.body248

vector.body248:                                   ; preds = %vector.body248, %vector.ph245
  %index249 = phi i64 [ 0, %vector.ph245 ], [ %index.next252, %vector.body248 ]
  %76 = getelementptr i8, ptr %71, i64 %index249
  %77 = getelementptr i8, ptr %.pre.i.i.i7, i64 %index249
  %78 = getelementptr i8, ptr %77, i64 16
  %wide.load250 = load <16 x i8>, ptr %77, align 1
  %wide.load251 = load <16 x i8>, ptr %78, align 1
  %79 = getelementptr i8, ptr %76, i64 16
  store <16 x i8> %wide.load250, ptr %76, align 1
  store <16 x i8> %wide.load251, ptr %79, align 1
  %index.next252 = add nuw i64 %index249, 32
  %80 = icmp eq i64 %index.next252, %n.vec247
  br i1 %80, label %middle.block239, label %vector.body248, !llvm.loop !24

middle.block239:                                  ; preds = %vector.body248
  %cmp.n253 = icmp eq i64 %65, %n.vec247
  br i1 %cmp.n253, label %.preheader.i.i.i8, label %vec.epilog.iter.check257

vec.epilog.iter.check257:                         ; preds = %middle.block239
  %n.vec.remaining258 = and i64 %65, 24
  %min.epilog.iters.check259 = icmp eq i64 %n.vec.remaining258, 0
  br i1 %min.epilog.iters.check259, label %.lr.ph15.i.i.i10.preheader, label %vec.epilog.ph256

.lr.ph15.i.i.i10.preheader:                       ; preds = %vec.epilog.middle.block254, %iter.check242, %vec.epilog.iter.check257
  %.114.i.i.i11.ph = phi i64 [ 0, %iter.check242 ], [ %n.vec247, %vec.epilog.iter.check257 ], [ %n.vec262, %vec.epilog.middle.block254 ]
  br label %.lr.ph15.i.i.i10

vec.epilog.ph256:                                 ; preds = %vector.main.loop.iter.check244, %vec.epilog.iter.check257
  %vec.epilog.resume.val260 = phi i64 [ %n.vec247, %vec.epilog.iter.check257 ], [ 0, %vector.main.loop.iter.check244 ]
  %n.vec262 = and i64 %65, 9223372036854775800
  br label %vec.epilog.vector.body264

vec.epilog.vector.body264:                        ; preds = %vec.epilog.vector.body264, %vec.epilog.ph256
  %index265 = phi i64 [ %vec.epilog.resume.val260, %vec.epilog.ph256 ], [ %index.next267, %vec.epilog.vector.body264 ]
  %81 = getelementptr i8, ptr %71, i64 %index265
  %82 = getelementptr i8, ptr %.pre.i.i.i7, i64 %index265
  %wide.load266 = load <8 x i8>, ptr %82, align 1
  store <8 x i8> %wide.load266, ptr %81, align 1
  %index.next267 = add nuw i64 %index265, 8
  %83 = icmp eq i64 %index.next267, %n.vec262
  br i1 %83, label %vec.epilog.middle.block254, label %vec.epilog.vector.body264, !llvm.loop !25

vec.epilog.middle.block254:                       ; preds = %vec.epilog.vector.body264
  %cmp.n268 = icmp eq i64 %65, %n.vec262
  br i1 %cmp.n268, label %.preheader.i.i.i8, label %.lr.ph15.i.i.i10.preheader

.preheader.i.i.i8:                                ; preds = %.lr.ph15.i.i.i10, %middle.block239, %vec.epilog.middle.block254, %.preheader12.i.i.i6
  tail call void @free(ptr %.pre.i.i.i7)
  store i64 %70, ptr %6, align 8
  store ptr %71, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit

.lr.ph15.i.i.i10:                                 ; preds = %.lr.ph15.i.i.i10.preheader, %.lr.ph15.i.i.i10
  %.114.i.i.i11 = phi i64 [ %87, %.lr.ph15.i.i.i10 ], [ %.114.i.i.i11.ph, %.lr.ph15.i.i.i10.preheader ]
  %84 = getelementptr i8, ptr %71, i64 %.114.i.i.i11
  %85 = getelementptr i8, ptr %.pre.i.i.i7, i64 %.114.i.i.i11
  %86 = load i8, ptr %85, align 1
  store i8 %86, ptr %84, align 1
  %87 = add nuw nsw i64 %.114.i.i.i11, 1
  %88 = icmp slt i64 %87, %65
  br i1 %88, label %.lr.ph15.i.i.i10, label %.preheader.i.i.i8, !llvm.loop !26

rl_m_append__String_int8_t.exit:                  ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i13, %.preheader.i.i.i8
  %89 = phi ptr [ %.pre2.i.i14, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i13 ], [ %71, %.preheader.i.i.i8 ]
  %90 = getelementptr i8, ptr %89, i64 %65
  store i8 0, ptr %90, align 1
  %91 = load i64, ptr %5, align 8
  %92 = add i64 %91, 1
  store i64 %92, ptr %5, align 8
  br label %rl__indent_string__String_int64_t.exit94

rl_m_get__String_int64_t_r_int8_tRef.exit15:      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %93 = load i64, ptr %5, align 8
  %94 = icmp sgt i64 %93, 0
  br i1 %94, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16, label %95

95:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit15
  %96 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16:   ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit15
  %97 = load ptr, ptr %4, align 8
  %98 = getelementptr i8, ptr %97, i64 %93
  %99 = getelementptr i8, ptr %98, i64 -1
  store i8 44, ptr %99, align 1
  %100 = load i64, ptr %5, align 8
  %101 = add i64 %100, 1
  %102 = load i64, ptr %6, align 8
  %103 = icmp sgt i64 %102, %101
  br i1 %103, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i24, label %104

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i24: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16
  %.pre2.i.i25 = load ptr, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit26

104:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16
  %105 = shl i64 %101, 1
  %106 = tail call ptr @malloc(i64 %105)
  %107 = ptrtoint ptr %106 to i64
  %108 = icmp sgt i64 %105, 0
  br i1 %108, label %.lr.ph.preheader.i.i.i23, label %.preheader12.i.i.i17

.lr.ph.preheader.i.i.i23:                         ; preds = %104
  tail call void @llvm.memset.p0.i64(ptr align 1 %106, i8 0, i64 %105, i1 false)
  br label %.preheader12.i.i.i17

.preheader12.i.i.i17:                             ; preds = %.lr.ph.preheader.i.i.i23, %104
  %109 = icmp sgt i64 %100, 0
  %.pre.i.i.i18 = load ptr, ptr %4, align 8
  br i1 %109, label %iter.check638, label %.preheader.i.i.i19

iter.check638:                                    ; preds = %.preheader12.i.i.i17
  %.pre.i.i.i18633 = ptrtoint ptr %.pre.i.i.i18 to i64
  %min.iters.check636 = icmp ult i64 %100, 8
  %110 = sub i64 %107, %.pre.i.i.i18633
  %diff.check634 = icmp ult i64 %110, 32
  %or.cond699 = select i1 %min.iters.check636, i1 true, i1 %diff.check634
  br i1 %or.cond699, label %.lr.ph15.i.i.i21.preheader, label %vector.main.loop.iter.check640

vector.main.loop.iter.check640:                   ; preds = %iter.check638
  %min.iters.check639 = icmp ult i64 %100, 32
  br i1 %min.iters.check639, label %vec.epilog.ph652, label %vector.ph641

vector.ph641:                                     ; preds = %vector.main.loop.iter.check640
  %n.vec643 = and i64 %100, 9223372036854775776
  br label %vector.body644

vector.body644:                                   ; preds = %vector.body644, %vector.ph641
  %index645 = phi i64 [ 0, %vector.ph641 ], [ %index.next648, %vector.body644 ]
  %111 = getelementptr i8, ptr %106, i64 %index645
  %112 = getelementptr i8, ptr %.pre.i.i.i18, i64 %index645
  %113 = getelementptr i8, ptr %112, i64 16
  %wide.load646 = load <16 x i8>, ptr %112, align 1
  %wide.load647 = load <16 x i8>, ptr %113, align 1
  %114 = getelementptr i8, ptr %111, i64 16
  store <16 x i8> %wide.load646, ptr %111, align 1
  store <16 x i8> %wide.load647, ptr %114, align 1
  %index.next648 = add nuw i64 %index645, 32
  %115 = icmp eq i64 %index.next648, %n.vec643
  br i1 %115, label %middle.block635, label %vector.body644, !llvm.loop !27

middle.block635:                                  ; preds = %vector.body644
  %cmp.n649 = icmp eq i64 %100, %n.vec643
  br i1 %cmp.n649, label %.preheader.i.i.i19, label %vec.epilog.iter.check653

vec.epilog.iter.check653:                         ; preds = %middle.block635
  %n.vec.remaining654 = and i64 %100, 24
  %min.epilog.iters.check655 = icmp eq i64 %n.vec.remaining654, 0
  br i1 %min.epilog.iters.check655, label %.lr.ph15.i.i.i21.preheader, label %vec.epilog.ph652

.lr.ph15.i.i.i21.preheader:                       ; preds = %vec.epilog.middle.block650, %iter.check638, %vec.epilog.iter.check653
  %.114.i.i.i22.ph = phi i64 [ 0, %iter.check638 ], [ %n.vec643, %vec.epilog.iter.check653 ], [ %n.vec658, %vec.epilog.middle.block650 ]
  br label %.lr.ph15.i.i.i21

vec.epilog.ph652:                                 ; preds = %vector.main.loop.iter.check640, %vec.epilog.iter.check653
  %vec.epilog.resume.val656 = phi i64 [ %n.vec643, %vec.epilog.iter.check653 ], [ 0, %vector.main.loop.iter.check640 ]
  %n.vec658 = and i64 %100, 9223372036854775800
  br label %vec.epilog.vector.body660

vec.epilog.vector.body660:                        ; preds = %vec.epilog.vector.body660, %vec.epilog.ph652
  %index661 = phi i64 [ %vec.epilog.resume.val656, %vec.epilog.ph652 ], [ %index.next663, %vec.epilog.vector.body660 ]
  %116 = getelementptr i8, ptr %106, i64 %index661
  %117 = getelementptr i8, ptr %.pre.i.i.i18, i64 %index661
  %wide.load662 = load <8 x i8>, ptr %117, align 1
  store <8 x i8> %wide.load662, ptr %116, align 1
  %index.next663 = add nuw i64 %index661, 8
  %118 = icmp eq i64 %index.next663, %n.vec658
  br i1 %118, label %vec.epilog.middle.block650, label %vec.epilog.vector.body660, !llvm.loop !28

vec.epilog.middle.block650:                       ; preds = %vec.epilog.vector.body660
  %cmp.n664 = icmp eq i64 %100, %n.vec658
  br i1 %cmp.n664, label %.preheader.i.i.i19, label %.lr.ph15.i.i.i21.preheader

.preheader.i.i.i19:                               ; preds = %.lr.ph15.i.i.i21, %middle.block635, %vec.epilog.middle.block650, %.preheader12.i.i.i17
  tail call void @free(ptr %.pre.i.i.i18)
  store i64 %105, ptr %6, align 8
  store ptr %106, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit26

.lr.ph15.i.i.i21:                                 ; preds = %.lr.ph15.i.i.i21.preheader, %.lr.ph15.i.i.i21
  %.114.i.i.i22 = phi i64 [ %122, %.lr.ph15.i.i.i21 ], [ %.114.i.i.i22.ph, %.lr.ph15.i.i.i21.preheader ]
  %119 = getelementptr i8, ptr %106, i64 %.114.i.i.i22
  %120 = getelementptr i8, ptr %.pre.i.i.i18, i64 %.114.i.i.i22
  %121 = load i8, ptr %120, align 1
  store i8 %121, ptr %119, align 1
  %122 = add nuw nsw i64 %.114.i.i.i22, 1
  %123 = icmp slt i64 %122, %100
  br i1 %123, label %.lr.ph15.i.i.i21, label %.preheader.i.i.i19, !llvm.loop !29

rl_m_append__String_int8_t.exit26:                ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i24, %.preheader.i.i.i19
  %124 = phi ptr [ %.pre2.i.i25, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i24 ], [ %106, %.preheader.i.i.i19 ]
  %125 = getelementptr i8, ptr %124, i64 %100
  store i8 0, ptr %125, align 1
  %126 = load i64, ptr %5, align 8
  %127 = add i64 %126, 1
  store i64 %127, ptr %5, align 8
  %128 = icmp ult i64 %126, 9223372036854775807
  br i1 %128, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27, label %129

129:                                              ; preds = %rl_m_append__String_int8_t.exit26
  %130 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27:   ; preds = %rl_m_append__String_int8_t.exit26
  %131 = load ptr, ptr %4, align 8
  %132 = getelementptr i8, ptr %131, i64 %127
  %133 = getelementptr i8, ptr %132, i64 -1
  store i8 10, ptr %133, align 1
  %134 = load i64, ptr %5, align 8
  %135 = add i64 %134, 1
  %136 = load i64, ptr %6, align 8
  %137 = icmp sgt i64 %136, %135
  br i1 %137, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i35, label %138

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i35: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27
  %.pre2.i.i36 = load ptr, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit37

138:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27
  %139 = shl i64 %135, 1
  %140 = tail call ptr @malloc(i64 %139)
  %141 = ptrtoint ptr %140 to i64
  %142 = icmp sgt i64 %139, 0
  br i1 %142, label %.lr.ph.preheader.i.i.i34, label %.preheader12.i.i.i28

.lr.ph.preheader.i.i.i34:                         ; preds = %138
  tail call void @llvm.memset.p0.i64(ptr align 1 %140, i8 0, i64 %139, i1 false)
  br label %.preheader12.i.i.i28

.preheader12.i.i.i28:                             ; preds = %.lr.ph.preheader.i.i.i34, %138
  %143 = icmp sgt i64 %134, 0
  %.pre.i.i.i29 = load ptr, ptr %4, align 8
  br i1 %143, label %iter.check605, label %.preheader.i.i.i30

iter.check605:                                    ; preds = %.preheader12.i.i.i28
  %.pre.i.i.i29600 = ptrtoint ptr %.pre.i.i.i29 to i64
  %min.iters.check603 = icmp ult i64 %134, 8
  %144 = sub i64 %141, %.pre.i.i.i29600
  %diff.check601 = icmp ult i64 %144, 32
  %or.cond700 = select i1 %min.iters.check603, i1 true, i1 %diff.check601
  br i1 %or.cond700, label %.lr.ph15.i.i.i32.preheader, label %vector.main.loop.iter.check607

vector.main.loop.iter.check607:                   ; preds = %iter.check605
  %min.iters.check606 = icmp ult i64 %134, 32
  br i1 %min.iters.check606, label %vec.epilog.ph619, label %vector.ph608

vector.ph608:                                     ; preds = %vector.main.loop.iter.check607
  %n.vec610 = and i64 %134, 9223372036854775776
  br label %vector.body611

vector.body611:                                   ; preds = %vector.body611, %vector.ph608
  %index612 = phi i64 [ 0, %vector.ph608 ], [ %index.next615, %vector.body611 ]
  %145 = getelementptr i8, ptr %140, i64 %index612
  %146 = getelementptr i8, ptr %.pre.i.i.i29, i64 %index612
  %147 = getelementptr i8, ptr %146, i64 16
  %wide.load613 = load <16 x i8>, ptr %146, align 1
  %wide.load614 = load <16 x i8>, ptr %147, align 1
  %148 = getelementptr i8, ptr %145, i64 16
  store <16 x i8> %wide.load613, ptr %145, align 1
  store <16 x i8> %wide.load614, ptr %148, align 1
  %index.next615 = add nuw i64 %index612, 32
  %149 = icmp eq i64 %index.next615, %n.vec610
  br i1 %149, label %middle.block602, label %vector.body611, !llvm.loop !30

middle.block602:                                  ; preds = %vector.body611
  %cmp.n616 = icmp eq i64 %134, %n.vec610
  br i1 %cmp.n616, label %.preheader.i.i.i30, label %vec.epilog.iter.check620

vec.epilog.iter.check620:                         ; preds = %middle.block602
  %n.vec.remaining621 = and i64 %134, 24
  %min.epilog.iters.check622 = icmp eq i64 %n.vec.remaining621, 0
  br i1 %min.epilog.iters.check622, label %.lr.ph15.i.i.i32.preheader, label %vec.epilog.ph619

.lr.ph15.i.i.i32.preheader:                       ; preds = %vec.epilog.middle.block617, %iter.check605, %vec.epilog.iter.check620
  %.114.i.i.i33.ph = phi i64 [ 0, %iter.check605 ], [ %n.vec610, %vec.epilog.iter.check620 ], [ %n.vec625, %vec.epilog.middle.block617 ]
  br label %.lr.ph15.i.i.i32

vec.epilog.ph619:                                 ; preds = %vector.main.loop.iter.check607, %vec.epilog.iter.check620
  %vec.epilog.resume.val623 = phi i64 [ %n.vec610, %vec.epilog.iter.check620 ], [ 0, %vector.main.loop.iter.check607 ]
  %n.vec625 = and i64 %134, 9223372036854775800
  br label %vec.epilog.vector.body627

vec.epilog.vector.body627:                        ; preds = %vec.epilog.vector.body627, %vec.epilog.ph619
  %index628 = phi i64 [ %vec.epilog.resume.val623, %vec.epilog.ph619 ], [ %index.next630, %vec.epilog.vector.body627 ]
  %150 = getelementptr i8, ptr %140, i64 %index628
  %151 = getelementptr i8, ptr %.pre.i.i.i29, i64 %index628
  %wide.load629 = load <8 x i8>, ptr %151, align 1
  store <8 x i8> %wide.load629, ptr %150, align 1
  %index.next630 = add nuw i64 %index628, 8
  %152 = icmp eq i64 %index.next630, %n.vec625
  br i1 %152, label %vec.epilog.middle.block617, label %vec.epilog.vector.body627, !llvm.loop !31

vec.epilog.middle.block617:                       ; preds = %vec.epilog.vector.body627
  %cmp.n631 = icmp eq i64 %134, %n.vec625
  br i1 %cmp.n631, label %.preheader.i.i.i30, label %.lr.ph15.i.i.i32.preheader

.preheader.i.i.i30:                               ; preds = %.lr.ph15.i.i.i32, %middle.block602, %vec.epilog.middle.block617, %.preheader12.i.i.i28
  tail call void @free(ptr %.pre.i.i.i29)
  store i64 %139, ptr %6, align 8
  store ptr %140, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit37

.lr.ph15.i.i.i32:                                 ; preds = %.lr.ph15.i.i.i32.preheader, %.lr.ph15.i.i.i32
  %.114.i.i.i33 = phi i64 [ %156, %.lr.ph15.i.i.i32 ], [ %.114.i.i.i33.ph, %.lr.ph15.i.i.i32.preheader ]
  %153 = getelementptr i8, ptr %140, i64 %.114.i.i.i33
  %154 = getelementptr i8, ptr %.pre.i.i.i29, i64 %.114.i.i.i33
  %155 = load i8, ptr %154, align 1
  store i8 %155, ptr %153, align 1
  %156 = add nuw nsw i64 %.114.i.i.i33, 1
  %157 = icmp slt i64 %156, %134
  br i1 %157, label %.lr.ph15.i.i.i32, label %.preheader.i.i.i30, !llvm.loop !32

rl_m_append__String_int8_t.exit37:                ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i35, %.preheader.i.i.i30
  %158 = phi ptr [ %.pre2.i.i36, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i35 ], [ %140, %.preheader.i.i.i30 ]
  %159 = getelementptr i8, ptr %158, i64 %134
  store i8 0, ptr %159, align 1
  %160 = load i64, ptr %5, align 8
  %161 = add i64 %160, 1
  store i64 %161, ptr %5, align 8
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %3)
  %.not2.i = icmp eq i64 %.0204216, 0
  br i1 %.not2.i, label %rl__indent_string__String_int64_t.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %rl_m_append__String_int8_t.exit37, %.lr.ph.i
  %.03.i = phi i64 [ %162, %.lr.ph.i ], [ 0, %rl_m_append__String_int8_t.exit37 ]
  store ptr @str_13, ptr %3, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %4, ptr nonnull %3)
  %162 = add nuw i64 %.03.i, 1
  %.not.i = icmp eq i64 %162, %.0204216
  br i1 %.not.i, label %rl__indent_string__String_int64_t.exit, label %.lr.ph.i

rl__indent_string__String_int64_t.exit:           ; preds = %.lr.ph.i, %rl_m_append__String_int8_t.exit37
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %3)
  %163 = add nuw i64 %.0217, 1
  %164 = icmp sgt i64 %163, -1
  br i1 %164, label %167, label %165

165:                                              ; preds = %rl__indent_string__String_int64_t.exit
  %166 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

167:                                              ; preds = %rl__indent_string__String_int64_t.exit
  %168 = load i64, ptr %40, align 8
  %169 = icmp slt i64 %163, %168
  br i1 %169, label %rl_m_get__String_int64_t_r_int8_tRef.exit38, label %170

170:                                              ; preds = %167
  %171 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit38:      ; preds = %167
  %172 = load ptr, ptr %1, align 8
  %173 = getelementptr i8, ptr %172, i64 %163
  %174 = load i8, ptr %173, align 1
  %175 = icmp eq i8 %174, 32
  %spec.select = select i1 %175, i64 %163, i64 %.0217
  br label %rl__indent_string__String_int64_t.exit94

rl_is_close_paren__int8_t_r_bool.exit.thread:     ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %176 = load i64, ptr %5, align 8
  %177 = icmp sgt i64 %176, 0
  br i1 %177, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39, label %178

178:                                              ; preds = %rl_is_close_paren__int8_t_r_bool.exit.thread
  %179 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39:   ; preds = %rl_is_close_paren__int8_t_r_bool.exit.thread
  %180 = load ptr, ptr %4, align 8
  %181 = getelementptr i8, ptr %180, i64 %176
  %182 = getelementptr i8, ptr %181, i64 -1
  store i8 10, ptr %182, align 1
  %183 = load i64, ptr %5, align 8
  %184 = add i64 %183, 1
  %185 = load i64, ptr %6, align 8
  %186 = icmp sgt i64 %185, %184
  br i1 %186, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i47, label %187

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i47: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39
  %.pre2.i.i48 = load ptr, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit49

187:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39
  %188 = shl i64 %184, 1
  %189 = tail call ptr @malloc(i64 %188)
  %190 = ptrtoint ptr %189 to i64
  %191 = icmp sgt i64 %188, 0
  br i1 %191, label %.lr.ph.preheader.i.i.i46, label %.preheader12.i.i.i40

.lr.ph.preheader.i.i.i46:                         ; preds = %187
  tail call void @llvm.memset.p0.i64(ptr align 1 %189, i8 0, i64 %188, i1 false)
  br label %.preheader12.i.i.i40

.preheader12.i.i.i40:                             ; preds = %.lr.ph.preheader.i.i.i46, %187
  %192 = icmp sgt i64 %183, 0
  %.pre.i.i.i41 = load ptr, ptr %4, align 8
  br i1 %192, label %iter.check572, label %.preheader.i.i.i42

iter.check572:                                    ; preds = %.preheader12.i.i.i40
  %.pre.i.i.i41567 = ptrtoint ptr %.pre.i.i.i41 to i64
  %min.iters.check570 = icmp ult i64 %183, 8
  %193 = sub i64 %190, %.pre.i.i.i41567
  %diff.check568 = icmp ult i64 %193, 32
  %or.cond701 = select i1 %min.iters.check570, i1 true, i1 %diff.check568
  br i1 %or.cond701, label %.lr.ph15.i.i.i44.preheader, label %vector.main.loop.iter.check574

vector.main.loop.iter.check574:                   ; preds = %iter.check572
  %min.iters.check573 = icmp ult i64 %183, 32
  br i1 %min.iters.check573, label %vec.epilog.ph586, label %vector.ph575

vector.ph575:                                     ; preds = %vector.main.loop.iter.check574
  %n.vec577 = and i64 %183, 9223372036854775776
  br label %vector.body578

vector.body578:                                   ; preds = %vector.body578, %vector.ph575
  %index579 = phi i64 [ 0, %vector.ph575 ], [ %index.next582, %vector.body578 ]
  %194 = getelementptr i8, ptr %189, i64 %index579
  %195 = getelementptr i8, ptr %.pre.i.i.i41, i64 %index579
  %196 = getelementptr i8, ptr %195, i64 16
  %wide.load580 = load <16 x i8>, ptr %195, align 1
  %wide.load581 = load <16 x i8>, ptr %196, align 1
  %197 = getelementptr i8, ptr %194, i64 16
  store <16 x i8> %wide.load580, ptr %194, align 1
  store <16 x i8> %wide.load581, ptr %197, align 1
  %index.next582 = add nuw i64 %index579, 32
  %198 = icmp eq i64 %index.next582, %n.vec577
  br i1 %198, label %middle.block569, label %vector.body578, !llvm.loop !33

middle.block569:                                  ; preds = %vector.body578
  %cmp.n583 = icmp eq i64 %183, %n.vec577
  br i1 %cmp.n583, label %.preheader.i.i.i42, label %vec.epilog.iter.check587

vec.epilog.iter.check587:                         ; preds = %middle.block569
  %n.vec.remaining588 = and i64 %183, 24
  %min.epilog.iters.check589 = icmp eq i64 %n.vec.remaining588, 0
  br i1 %min.epilog.iters.check589, label %.lr.ph15.i.i.i44.preheader, label %vec.epilog.ph586

.lr.ph15.i.i.i44.preheader:                       ; preds = %vec.epilog.middle.block584, %iter.check572, %vec.epilog.iter.check587
  %.114.i.i.i45.ph = phi i64 [ 0, %iter.check572 ], [ %n.vec577, %vec.epilog.iter.check587 ], [ %n.vec592, %vec.epilog.middle.block584 ]
  br label %.lr.ph15.i.i.i44

vec.epilog.ph586:                                 ; preds = %vector.main.loop.iter.check574, %vec.epilog.iter.check587
  %vec.epilog.resume.val590 = phi i64 [ %n.vec577, %vec.epilog.iter.check587 ], [ 0, %vector.main.loop.iter.check574 ]
  %n.vec592 = and i64 %183, 9223372036854775800
  br label %vec.epilog.vector.body594

vec.epilog.vector.body594:                        ; preds = %vec.epilog.vector.body594, %vec.epilog.ph586
  %index595 = phi i64 [ %vec.epilog.resume.val590, %vec.epilog.ph586 ], [ %index.next597, %vec.epilog.vector.body594 ]
  %199 = getelementptr i8, ptr %189, i64 %index595
  %200 = getelementptr i8, ptr %.pre.i.i.i41, i64 %index595
  %wide.load596 = load <8 x i8>, ptr %200, align 1
  store <8 x i8> %wide.load596, ptr %199, align 1
  %index.next597 = add nuw i64 %index595, 8
  %201 = icmp eq i64 %index.next597, %n.vec592
  br i1 %201, label %vec.epilog.middle.block584, label %vec.epilog.vector.body594, !llvm.loop !34

vec.epilog.middle.block584:                       ; preds = %vec.epilog.vector.body594
  %cmp.n598 = icmp eq i64 %183, %n.vec592
  br i1 %cmp.n598, label %.preheader.i.i.i42, label %.lr.ph15.i.i.i44.preheader

.preheader.i.i.i42:                               ; preds = %.lr.ph15.i.i.i44, %middle.block569, %vec.epilog.middle.block584, %.preheader12.i.i.i40
  tail call void @free(ptr %.pre.i.i.i41)
  store i64 %188, ptr %6, align 8
  store ptr %189, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit49

.lr.ph15.i.i.i44:                                 ; preds = %.lr.ph15.i.i.i44.preheader, %.lr.ph15.i.i.i44
  %.114.i.i.i45 = phi i64 [ %205, %.lr.ph15.i.i.i44 ], [ %.114.i.i.i45.ph, %.lr.ph15.i.i.i44.preheader ]
  %202 = getelementptr i8, ptr %189, i64 %.114.i.i.i45
  %203 = getelementptr i8, ptr %.pre.i.i.i41, i64 %.114.i.i.i45
  %204 = load i8, ptr %203, align 1
  store i8 %204, ptr %202, align 1
  %205 = add nuw nsw i64 %.114.i.i.i45, 1
  %206 = icmp slt i64 %205, %183
  br i1 %206, label %.lr.ph15.i.i.i44, label %.preheader.i.i.i42, !llvm.loop !35

rl_m_append__String_int8_t.exit49:                ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i47, %.preheader.i.i.i42
  %207 = phi ptr [ %.pre2.i.i48, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i47 ], [ %189, %.preheader.i.i.i42 ]
  %208 = getelementptr i8, ptr %207, i64 %183
  store i8 0, ptr %208, align 1
  %209 = load i64, ptr %5, align 8
  %210 = add i64 %209, 1
  store i64 %210, ptr %5, align 8
  %211 = add i64 %.0204216, -1
  %.not2.i50 = icmp eq i64 %211, 0
  br i1 %.not2.i50, label %rl__indent_string__String_int64_t.exit54, label %.lr.ph.i51

.lr.ph.i51:                                       ; preds = %rl_m_append__String_int8_t.exit49, %rl_m_append__String_strlit.exit
  %212 = phi i64 [ %299, %rl_m_append__String_strlit.exit ], [ %210, %rl_m_append__String_int8_t.exit49 ]
  %.03.i52 = phi i64 [ %300, %rl_m_append__String_strlit.exit ], [ 0, %rl_m_append__String_int8_t.exit49 ]
  %213 = icmp sgt i64 %212, 0
  br i1 %213, label %.lr.ph.i108, label %214

214:                                              ; preds = %.lr.ph.i51
  %215 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

.lr.ph.i108:                                      ; preds = %.lr.ph.i51
  %216 = add nsw i64 %212, -1
  %217 = load ptr, ptr %4, align 8
  %218 = getelementptr i8, ptr %217, i64 %216
  store i64 %216, ptr %5, align 8
  store i8 0, ptr %218, align 1
  %.pre16.i = load i64, ptr %5, align 8
  %219 = add i64 %.pre16.i, 1
  %220 = load i64, ptr %6, align 8
  %221 = icmp sgt i64 %220, %219
  br i1 %221, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i117, label %222

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i117: ; preds = %.lr.ph.i108
  %.pre2.i.i118 = load ptr, ptr %4, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i

222:                                              ; preds = %.lr.ph.i108
  %223 = shl i64 %219, 1
  %224 = tail call ptr @malloc(i64 %223)
  %225 = ptrtoint ptr %224 to i64
  %226 = icmp sgt i64 %223, 0
  br i1 %226, label %.lr.ph.preheader.i.i.i116, label %.preheader12.i.i.i109

.lr.ph.preheader.i.i.i116:                        ; preds = %222
  tail call void @llvm.memset.p0.i64(ptr align 1 %224, i8 0, i64 %223, i1 false)
  br label %.preheader12.i.i.i109

.preheader12.i.i.i109:                            ; preds = %.lr.ph.preheader.i.i.i116, %222
  %227 = icmp sgt i64 %.pre16.i, 0
  %.pre.i.i.i110 = load ptr, ptr %4, align 8
  br i1 %227, label %iter.check539, label %.preheader.i.i.i111

iter.check539:                                    ; preds = %.preheader12.i.i.i109
  %.pre.i.i.i110534 = ptrtoint ptr %.pre.i.i.i110 to i64
  %min.iters.check537 = icmp ult i64 %.pre16.i, 8
  %228 = sub i64 %225, %.pre.i.i.i110534
  %diff.check535 = icmp ult i64 %228, 32
  %or.cond702 = select i1 %min.iters.check537, i1 true, i1 %diff.check535
  br i1 %or.cond702, label %.lr.ph15.i.i.i114.preheader, label %vector.main.loop.iter.check541

vector.main.loop.iter.check541:                   ; preds = %iter.check539
  %min.iters.check540 = icmp ult i64 %.pre16.i, 32
  br i1 %min.iters.check540, label %vec.epilog.ph553, label %vector.ph542

vector.ph542:                                     ; preds = %vector.main.loop.iter.check541
  %n.vec544 = and i64 %.pre16.i, 9223372036854775776
  br label %vector.body545

vector.body545:                                   ; preds = %vector.body545, %vector.ph542
  %index546 = phi i64 [ 0, %vector.ph542 ], [ %index.next549, %vector.body545 ]
  %229 = getelementptr i8, ptr %224, i64 %index546
  %230 = getelementptr i8, ptr %.pre.i.i.i110, i64 %index546
  %231 = getelementptr i8, ptr %230, i64 16
  %wide.load547 = load <16 x i8>, ptr %230, align 1
  %wide.load548 = load <16 x i8>, ptr %231, align 1
  %232 = getelementptr i8, ptr %229, i64 16
  store <16 x i8> %wide.load547, ptr %229, align 1
  store <16 x i8> %wide.load548, ptr %232, align 1
  %index.next549 = add nuw i64 %index546, 32
  %233 = icmp eq i64 %index.next549, %n.vec544
  br i1 %233, label %middle.block536, label %vector.body545, !llvm.loop !36

middle.block536:                                  ; preds = %vector.body545
  %cmp.n550 = icmp eq i64 %.pre16.i, %n.vec544
  br i1 %cmp.n550, label %.preheader.i.i.i111, label %vec.epilog.iter.check554

vec.epilog.iter.check554:                         ; preds = %middle.block536
  %n.vec.remaining555 = and i64 %.pre16.i, 24
  %min.epilog.iters.check556 = icmp eq i64 %n.vec.remaining555, 0
  br i1 %min.epilog.iters.check556, label %.lr.ph15.i.i.i114.preheader, label %vec.epilog.ph553

.lr.ph15.i.i.i114.preheader:                      ; preds = %vec.epilog.middle.block551, %iter.check539, %vec.epilog.iter.check554
  %.114.i.i.i115.ph = phi i64 [ 0, %iter.check539 ], [ %n.vec544, %vec.epilog.iter.check554 ], [ %n.vec559, %vec.epilog.middle.block551 ]
  br label %.lr.ph15.i.i.i114

vec.epilog.ph553:                                 ; preds = %vector.main.loop.iter.check541, %vec.epilog.iter.check554
  %vec.epilog.resume.val557 = phi i64 [ %n.vec544, %vec.epilog.iter.check554 ], [ 0, %vector.main.loop.iter.check541 ]
  %n.vec559 = and i64 %.pre16.i, 9223372036854775800
  br label %vec.epilog.vector.body561

vec.epilog.vector.body561:                        ; preds = %vec.epilog.vector.body561, %vec.epilog.ph553
  %index562 = phi i64 [ %vec.epilog.resume.val557, %vec.epilog.ph553 ], [ %index.next564, %vec.epilog.vector.body561 ]
  %234 = getelementptr i8, ptr %224, i64 %index562
  %235 = getelementptr i8, ptr %.pre.i.i.i110, i64 %index562
  %wide.load563 = load <8 x i8>, ptr %235, align 1
  store <8 x i8> %wide.load563, ptr %234, align 1
  %index.next564 = add nuw i64 %index562, 8
  %236 = icmp eq i64 %index.next564, %n.vec559
  br i1 %236, label %vec.epilog.middle.block551, label %vec.epilog.vector.body561, !llvm.loop !37

vec.epilog.middle.block551:                       ; preds = %vec.epilog.vector.body561
  %cmp.n565 = icmp eq i64 %.pre16.i, %n.vec559
  br i1 %cmp.n565, label %.preheader.i.i.i111, label %.lr.ph15.i.i.i114.preheader

.preheader.i.i.i111:                              ; preds = %.lr.ph15.i.i.i114, %middle.block536, %vec.epilog.middle.block551, %.preheader12.i.i.i109
  tail call void @free(ptr %.pre.i.i.i110)
  store i64 %223, ptr %6, align 8
  store ptr %224, ptr %4, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i

.lr.ph15.i.i.i114:                                ; preds = %.lr.ph15.i.i.i114.preheader, %.lr.ph15.i.i.i114
  %.114.i.i.i115 = phi i64 [ %240, %.lr.ph15.i.i.i114 ], [ %.114.i.i.i115.ph, %.lr.ph15.i.i.i114.preheader ]
  %237 = getelementptr i8, ptr %224, i64 %.114.i.i.i115
  %238 = getelementptr i8, ptr %.pre.i.i.i110, i64 %.114.i.i.i115
  %239 = load i8, ptr %238, align 1
  store i8 %239, ptr %237, align 1
  %240 = add nuw nsw i64 %.114.i.i.i115, 1
  %241 = icmp slt i64 %240, %.pre16.i
  br i1 %241, label %.lr.ph15.i.i.i114, label %.preheader.i.i.i111, !llvm.loop !38

rl_m_append__VectorTint8_tT_int8_t.exit.i:        ; preds = %.preheader.i.i.i111, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i117
  %242 = phi ptr [ %.pre2.i.i118, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i117 ], [ %224, %.preheader.i.i.i111 ]
  %243 = getelementptr i8, ptr %242, i64 %.pre16.i
  store i8 32, ptr %243, align 1
  %244 = load i64, ptr %5, align 8
  %245 = add i64 %244, 1
  store i64 %245, ptr %5, align 8
  %246 = add i64 %244, 2
  %247 = load i64, ptr %6, align 8
  %248 = icmp sgt i64 %247, %246
  br i1 %248, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i117.1, label %249

249:                                              ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i
  %250 = shl i64 %246, 1
  %251 = tail call ptr @malloc(i64 %250)
  %252 = ptrtoint ptr %251 to i64
  %253 = icmp sgt i64 %250, 0
  br i1 %253, label %.lr.ph.preheader.i.i.i116.1, label %.preheader12.i.i.i109.1

.lr.ph.preheader.i.i.i116.1:                      ; preds = %249
  tail call void @llvm.memset.p0.i64(ptr align 1 %251, i8 0, i64 %250, i1 false)
  br label %.preheader12.i.i.i109.1

.preheader12.i.i.i109.1:                          ; preds = %.lr.ph.preheader.i.i.i116.1, %249
  %254 = icmp ult i64 %244, 9223372036854775807
  %.pre.i.i.i110.1 = load ptr, ptr %4, align 8
  br i1 %254, label %iter.check506, label %.preheader.i.i.i111.1

iter.check506:                                    ; preds = %.preheader12.i.i.i109.1
  %.pre.i.i.i110.1501 = ptrtoint ptr %.pre.i.i.i110.1 to i64
  %min.iters.check504 = icmp ult i64 %245, 8
  %255 = sub i64 %252, %.pre.i.i.i110.1501
  %diff.check502 = icmp ult i64 %255, 32
  %or.cond703 = select i1 %min.iters.check504, i1 true, i1 %diff.check502
  br i1 %or.cond703, label %.lr.ph15.i.i.i114.1.preheader, label %vector.main.loop.iter.check508

vector.main.loop.iter.check508:                   ; preds = %iter.check506
  %min.iters.check507 = icmp ult i64 %245, 32
  br i1 %min.iters.check507, label %vec.epilog.ph520, label %vector.ph509

vector.ph509:                                     ; preds = %vector.main.loop.iter.check508
  %n.vec511 = and i64 %245, -32
  br label %vector.body512

vector.body512:                                   ; preds = %vector.body512, %vector.ph509
  %index513 = phi i64 [ 0, %vector.ph509 ], [ %index.next516, %vector.body512 ]
  %256 = getelementptr i8, ptr %251, i64 %index513
  %257 = getelementptr i8, ptr %.pre.i.i.i110.1, i64 %index513
  %258 = getelementptr i8, ptr %257, i64 16
  %wide.load514 = load <16 x i8>, ptr %257, align 1
  %wide.load515 = load <16 x i8>, ptr %258, align 1
  %259 = getelementptr i8, ptr %256, i64 16
  store <16 x i8> %wide.load514, ptr %256, align 1
  store <16 x i8> %wide.load515, ptr %259, align 1
  %index.next516 = add nuw i64 %index513, 32
  %260 = icmp eq i64 %index.next516, %n.vec511
  br i1 %260, label %middle.block503, label %vector.body512, !llvm.loop !39

middle.block503:                                  ; preds = %vector.body512
  %cmp.n517 = icmp eq i64 %245, %n.vec511
  br i1 %cmp.n517, label %.preheader.i.i.i111.1, label %vec.epilog.iter.check521

vec.epilog.iter.check521:                         ; preds = %middle.block503
  %n.vec.remaining522 = and i64 %245, 24
  %min.epilog.iters.check523 = icmp eq i64 %n.vec.remaining522, 0
  br i1 %min.epilog.iters.check523, label %.lr.ph15.i.i.i114.1.preheader, label %vec.epilog.ph520

vec.epilog.ph520:                                 ; preds = %vector.main.loop.iter.check508, %vec.epilog.iter.check521
  %vec.epilog.resume.val524 = phi i64 [ %n.vec511, %vec.epilog.iter.check521 ], [ 0, %vector.main.loop.iter.check508 ]
  %n.vec526 = and i64 %245, -8
  br label %vec.epilog.vector.body528

vec.epilog.vector.body528:                        ; preds = %vec.epilog.vector.body528, %vec.epilog.ph520
  %index529 = phi i64 [ %vec.epilog.resume.val524, %vec.epilog.ph520 ], [ %index.next531, %vec.epilog.vector.body528 ]
  %261 = getelementptr i8, ptr %251, i64 %index529
  %262 = getelementptr i8, ptr %.pre.i.i.i110.1, i64 %index529
  %wide.load530 = load <8 x i8>, ptr %262, align 1
  store <8 x i8> %wide.load530, ptr %261, align 1
  %index.next531 = add nuw i64 %index529, 8
  %263 = icmp eq i64 %index.next531, %n.vec526
  br i1 %263, label %vec.epilog.middle.block518, label %vec.epilog.vector.body528, !llvm.loop !40

vec.epilog.middle.block518:                       ; preds = %vec.epilog.vector.body528
  %cmp.n532 = icmp eq i64 %245, %n.vec526
  br i1 %cmp.n532, label %.preheader.i.i.i111.1, label %.lr.ph15.i.i.i114.1.preheader

.lr.ph15.i.i.i114.1.preheader:                    ; preds = %vec.epilog.middle.block518, %iter.check506, %vec.epilog.iter.check521
  %.114.i.i.i115.1.ph = phi i64 [ 0, %iter.check506 ], [ %n.vec511, %vec.epilog.iter.check521 ], [ %n.vec526, %vec.epilog.middle.block518 ]
  br label %.lr.ph15.i.i.i114.1

.lr.ph15.i.i.i114.1:                              ; preds = %.lr.ph15.i.i.i114.1.preheader, %.lr.ph15.i.i.i114.1
  %.114.i.i.i115.1 = phi i64 [ %267, %.lr.ph15.i.i.i114.1 ], [ %.114.i.i.i115.1.ph, %.lr.ph15.i.i.i114.1.preheader ]
  %264 = getelementptr i8, ptr %251, i64 %.114.i.i.i115.1
  %265 = getelementptr i8, ptr %.pre.i.i.i110.1, i64 %.114.i.i.i115.1
  %266 = load i8, ptr %265, align 1
  store i8 %266, ptr %264, align 1
  %267 = add nuw nsw i64 %.114.i.i.i115.1, 1
  %268 = icmp slt i64 %267, %245
  br i1 %268, label %.lr.ph15.i.i.i114.1, label %.preheader.i.i.i111.1, !llvm.loop !41

.preheader.i.i.i111.1:                            ; preds = %.lr.ph15.i.i.i114.1, %middle.block503, %vec.epilog.middle.block518, %.preheader12.i.i.i109.1
  tail call void @free(ptr %.pre.i.i.i110.1)
  store i64 %250, ptr %6, align 8
  store ptr %251, ptr %4, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i.1

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i117.1: ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i
  %.pre2.i.i118.1 = load ptr, ptr %4, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i.1

rl_m_append__VectorTint8_tT_int8_t.exit.i.1:      ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i117.1, %.preheader.i.i.i111.1
  %269 = phi ptr [ %.pre2.i.i118.1, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i117.1 ], [ %251, %.preheader.i.i.i111.1 ]
  %270 = getelementptr i8, ptr %269, i64 %245
  store i8 32, ptr %270, align 1
  %271 = load i64, ptr %5, align 8
  %272 = add i64 %271, 1
  store i64 %272, ptr %5, align 8
  %273 = add i64 %271, 2
  %274 = load i64, ptr %6, align 8
  %275 = icmp sgt i64 %274, %273
  br i1 %275, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10.i, label %276

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10.i: ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i.1
  %.pre2.i11.i = load ptr, ptr %4, align 8
  br label %rl_m_append__String_strlit.exit

276:                                              ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i.1
  %277 = shl i64 %273, 1
  %278 = tail call ptr @malloc(i64 %277)
  %279 = ptrtoint ptr %278 to i64
  %280 = icmp sgt i64 %277, 0
  br i1 %280, label %.lr.ph.preheader.i.i9.i, label %.preheader12.i.i3.i

.lr.ph.preheader.i.i9.i:                          ; preds = %276
  tail call void @llvm.memset.p0.i64(ptr align 1 %278, i8 0, i64 %277, i1 false)
  br label %.preheader12.i.i3.i

.preheader12.i.i3.i:                              ; preds = %.lr.ph.preheader.i.i9.i, %276
  %281 = icmp ult i64 %271, 9223372036854775807
  %.pre.i.i4.i = load ptr, ptr %4, align 8
  br i1 %281, label %iter.check473, label %.preheader.i.i5.i

iter.check473:                                    ; preds = %.preheader12.i.i3.i
  %.pre.i.i4.i468 = ptrtoint ptr %.pre.i.i4.i to i64
  %min.iters.check471 = icmp ult i64 %272, 8
  %282 = sub i64 %279, %.pre.i.i4.i468
  %diff.check469 = icmp ult i64 %282, 32
  %or.cond704 = select i1 %min.iters.check471, i1 true, i1 %diff.check469
  br i1 %or.cond704, label %.lr.ph15.i.i7.i.preheader, label %vector.main.loop.iter.check475

vector.main.loop.iter.check475:                   ; preds = %iter.check473
  %min.iters.check474 = icmp ult i64 %272, 32
  br i1 %min.iters.check474, label %vec.epilog.ph487, label %vector.ph476

vector.ph476:                                     ; preds = %vector.main.loop.iter.check475
  %n.vec478 = and i64 %272, -32
  br label %vector.body479

vector.body479:                                   ; preds = %vector.body479, %vector.ph476
  %index480 = phi i64 [ 0, %vector.ph476 ], [ %index.next483, %vector.body479 ]
  %283 = getelementptr i8, ptr %278, i64 %index480
  %284 = getelementptr i8, ptr %.pre.i.i4.i, i64 %index480
  %285 = getelementptr i8, ptr %284, i64 16
  %wide.load481 = load <16 x i8>, ptr %284, align 1
  %wide.load482 = load <16 x i8>, ptr %285, align 1
  %286 = getelementptr i8, ptr %283, i64 16
  store <16 x i8> %wide.load481, ptr %283, align 1
  store <16 x i8> %wide.load482, ptr %286, align 1
  %index.next483 = add nuw i64 %index480, 32
  %287 = icmp eq i64 %index.next483, %n.vec478
  br i1 %287, label %middle.block470, label %vector.body479, !llvm.loop !42

middle.block470:                                  ; preds = %vector.body479
  %cmp.n484 = icmp eq i64 %272, %n.vec478
  br i1 %cmp.n484, label %.preheader.i.i5.i, label %vec.epilog.iter.check488

vec.epilog.iter.check488:                         ; preds = %middle.block470
  %n.vec.remaining489 = and i64 %272, 24
  %min.epilog.iters.check490 = icmp eq i64 %n.vec.remaining489, 0
  br i1 %min.epilog.iters.check490, label %.lr.ph15.i.i7.i.preheader, label %vec.epilog.ph487

.lr.ph15.i.i7.i.preheader:                        ; preds = %vec.epilog.middle.block485, %iter.check473, %vec.epilog.iter.check488
  %.114.i.i8.i.ph = phi i64 [ 0, %iter.check473 ], [ %n.vec478, %vec.epilog.iter.check488 ], [ %n.vec493, %vec.epilog.middle.block485 ]
  br label %.lr.ph15.i.i7.i

vec.epilog.ph487:                                 ; preds = %vector.main.loop.iter.check475, %vec.epilog.iter.check488
  %vec.epilog.resume.val491 = phi i64 [ %n.vec478, %vec.epilog.iter.check488 ], [ 0, %vector.main.loop.iter.check475 ]
  %n.vec493 = and i64 %272, -8
  br label %vec.epilog.vector.body495

vec.epilog.vector.body495:                        ; preds = %vec.epilog.vector.body495, %vec.epilog.ph487
  %index496 = phi i64 [ %vec.epilog.resume.val491, %vec.epilog.ph487 ], [ %index.next498, %vec.epilog.vector.body495 ]
  %288 = getelementptr i8, ptr %278, i64 %index496
  %289 = getelementptr i8, ptr %.pre.i.i4.i, i64 %index496
  %wide.load497 = load <8 x i8>, ptr %289, align 1
  store <8 x i8> %wide.load497, ptr %288, align 1
  %index.next498 = add nuw i64 %index496, 8
  %290 = icmp eq i64 %index.next498, %n.vec493
  br i1 %290, label %vec.epilog.middle.block485, label %vec.epilog.vector.body495, !llvm.loop !43

vec.epilog.middle.block485:                       ; preds = %vec.epilog.vector.body495
  %cmp.n499 = icmp eq i64 %272, %n.vec493
  br i1 %cmp.n499, label %.preheader.i.i5.i, label %.lr.ph15.i.i7.i.preheader

.preheader.i.i5.i:                                ; preds = %.lr.ph15.i.i7.i, %middle.block470, %vec.epilog.middle.block485, %.preheader12.i.i3.i
  tail call void @free(ptr %.pre.i.i4.i)
  store i64 %277, ptr %6, align 8
  store ptr %278, ptr %4, align 8
  br label %rl_m_append__String_strlit.exit

.lr.ph15.i.i7.i:                                  ; preds = %.lr.ph15.i.i7.i.preheader, %.lr.ph15.i.i7.i
  %.114.i.i8.i = phi i64 [ %294, %.lr.ph15.i.i7.i ], [ %.114.i.i8.i.ph, %.lr.ph15.i.i7.i.preheader ]
  %291 = getelementptr i8, ptr %278, i64 %.114.i.i8.i
  %292 = getelementptr i8, ptr %.pre.i.i4.i, i64 %.114.i.i8.i
  %293 = load i8, ptr %292, align 1
  store i8 %293, ptr %291, align 1
  %294 = add nuw nsw i64 %.114.i.i8.i, 1
  %295 = icmp slt i64 %294, %272
  br i1 %295, label %.lr.ph15.i.i7.i, label %.preheader.i.i5.i, !llvm.loop !44

rl_m_append__String_strlit.exit:                  ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10.i, %.preheader.i.i5.i
  %296 = phi ptr [ %.pre2.i11.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10.i ], [ %278, %.preheader.i.i5.i ]
  %297 = getelementptr i8, ptr %296, i64 %272
  store i8 0, ptr %297, align 1
  %298 = load i64, ptr %5, align 8
  %299 = add i64 %298, 1
  store i64 %299, ptr %5, align 8
  %300 = add nuw i64 %.03.i52, 1
  %.not.i53 = icmp eq i64 %300, %211
  br i1 %.not.i53, label %rl__indent_string__String_int64_t.exit54, label %.lr.ph.i51

rl__indent_string__String_int64_t.exit54:         ; preds = %rl_m_append__String_strlit.exit, %rl_m_append__String_int8_t.exit49
  %301 = phi i64 [ %210, %rl_m_append__String_int8_t.exit49 ], [ %299, %rl_m_append__String_strlit.exit ]
  %302 = load i64, ptr %40, align 8
  %303 = icmp slt i64 %.0217, %302
  br i1 %303, label %rl_m_get__String_int64_t_r_int8_tRef.exit55, label %304

304:                                              ; preds = %rl__indent_string__String_int64_t.exit54
  %305 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit55:      ; preds = %rl__indent_string__String_int64_t.exit54
  %306 = icmp sgt i64 %301, 0
  br i1 %306, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56, label %307

307:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit55
  %308 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56:   ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit55
  %309 = load ptr, ptr %1, align 8
  %310 = getelementptr i8, ptr %309, i64 %.0217
  %311 = load ptr, ptr %4, align 8
  %312 = getelementptr i8, ptr %311, i64 %301
  %313 = getelementptr i8, ptr %312, i64 -1
  %314 = load i8, ptr %310, align 1
  store i8 %314, ptr %313, align 1
  %315 = load i64, ptr %5, align 8
  %316 = add i64 %315, 1
  %317 = load i64, ptr %6, align 8
  %318 = icmp sgt i64 %317, %316
  br i1 %318, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i64, label %319

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i64: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56
  %.pre2.i.i65 = load ptr, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit66

319:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56
  %320 = shl i64 %316, 1
  %321 = tail call ptr @malloc(i64 %320)
  %322 = ptrtoint ptr %321 to i64
  %323 = icmp sgt i64 %320, 0
  br i1 %323, label %.lr.ph.preheader.i.i.i63, label %.preheader12.i.i.i57

.lr.ph.preheader.i.i.i63:                         ; preds = %319
  tail call void @llvm.memset.p0.i64(ptr align 1 %321, i8 0, i64 %320, i1 false)
  br label %.preheader12.i.i.i57

.preheader12.i.i.i57:                             ; preds = %.lr.ph.preheader.i.i.i63, %319
  %324 = icmp sgt i64 %315, 0
  %.pre.i.i.i58 = load ptr, ptr %4, align 8
  br i1 %324, label %iter.check440, label %.preheader.i.i.i59

iter.check440:                                    ; preds = %.preheader12.i.i.i57
  %.pre.i.i.i58435 = ptrtoint ptr %.pre.i.i.i58 to i64
  %min.iters.check438 = icmp ult i64 %315, 8
  %325 = sub i64 %322, %.pre.i.i.i58435
  %diff.check436 = icmp ult i64 %325, 32
  %or.cond705 = select i1 %min.iters.check438, i1 true, i1 %diff.check436
  br i1 %or.cond705, label %.lr.ph15.i.i.i61.preheader, label %vector.main.loop.iter.check442

vector.main.loop.iter.check442:                   ; preds = %iter.check440
  %min.iters.check441 = icmp ult i64 %315, 32
  br i1 %min.iters.check441, label %vec.epilog.ph454, label %vector.ph443

vector.ph443:                                     ; preds = %vector.main.loop.iter.check442
  %n.vec445 = and i64 %315, 9223372036854775776
  br label %vector.body446

vector.body446:                                   ; preds = %vector.body446, %vector.ph443
  %index447 = phi i64 [ 0, %vector.ph443 ], [ %index.next450, %vector.body446 ]
  %326 = getelementptr i8, ptr %321, i64 %index447
  %327 = getelementptr i8, ptr %.pre.i.i.i58, i64 %index447
  %328 = getelementptr i8, ptr %327, i64 16
  %wide.load448 = load <16 x i8>, ptr %327, align 1
  %wide.load449 = load <16 x i8>, ptr %328, align 1
  %329 = getelementptr i8, ptr %326, i64 16
  store <16 x i8> %wide.load448, ptr %326, align 1
  store <16 x i8> %wide.load449, ptr %329, align 1
  %index.next450 = add nuw i64 %index447, 32
  %330 = icmp eq i64 %index.next450, %n.vec445
  br i1 %330, label %middle.block437, label %vector.body446, !llvm.loop !45

middle.block437:                                  ; preds = %vector.body446
  %cmp.n451 = icmp eq i64 %315, %n.vec445
  br i1 %cmp.n451, label %.preheader.i.i.i59, label %vec.epilog.iter.check455

vec.epilog.iter.check455:                         ; preds = %middle.block437
  %n.vec.remaining456 = and i64 %315, 24
  %min.epilog.iters.check457 = icmp eq i64 %n.vec.remaining456, 0
  br i1 %min.epilog.iters.check457, label %.lr.ph15.i.i.i61.preheader, label %vec.epilog.ph454

.lr.ph15.i.i.i61.preheader:                       ; preds = %vec.epilog.middle.block452, %iter.check440, %vec.epilog.iter.check455
  %.114.i.i.i62.ph = phi i64 [ 0, %iter.check440 ], [ %n.vec445, %vec.epilog.iter.check455 ], [ %n.vec460, %vec.epilog.middle.block452 ]
  br label %.lr.ph15.i.i.i61

vec.epilog.ph454:                                 ; preds = %vector.main.loop.iter.check442, %vec.epilog.iter.check455
  %vec.epilog.resume.val458 = phi i64 [ %n.vec445, %vec.epilog.iter.check455 ], [ 0, %vector.main.loop.iter.check442 ]
  %n.vec460 = and i64 %315, 9223372036854775800
  br label %vec.epilog.vector.body462

vec.epilog.vector.body462:                        ; preds = %vec.epilog.vector.body462, %vec.epilog.ph454
  %index463 = phi i64 [ %vec.epilog.resume.val458, %vec.epilog.ph454 ], [ %index.next465, %vec.epilog.vector.body462 ]
  %331 = getelementptr i8, ptr %321, i64 %index463
  %332 = getelementptr i8, ptr %.pre.i.i.i58, i64 %index463
  %wide.load464 = load <8 x i8>, ptr %332, align 1
  store <8 x i8> %wide.load464, ptr %331, align 1
  %index.next465 = add nuw i64 %index463, 8
  %333 = icmp eq i64 %index.next465, %n.vec460
  br i1 %333, label %vec.epilog.middle.block452, label %vec.epilog.vector.body462, !llvm.loop !46

vec.epilog.middle.block452:                       ; preds = %vec.epilog.vector.body462
  %cmp.n466 = icmp eq i64 %315, %n.vec460
  br i1 %cmp.n466, label %.preheader.i.i.i59, label %.lr.ph15.i.i.i61.preheader

.preheader.i.i.i59:                               ; preds = %.lr.ph15.i.i.i61, %middle.block437, %vec.epilog.middle.block452, %.preheader12.i.i.i57
  tail call void @free(ptr %.pre.i.i.i58)
  store i64 %320, ptr %6, align 8
  store ptr %321, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit66

.lr.ph15.i.i.i61:                                 ; preds = %.lr.ph15.i.i.i61.preheader, %.lr.ph15.i.i.i61
  %.114.i.i.i62 = phi i64 [ %337, %.lr.ph15.i.i.i61 ], [ %.114.i.i.i62.ph, %.lr.ph15.i.i.i61.preheader ]
  %334 = getelementptr i8, ptr %321, i64 %.114.i.i.i62
  %335 = getelementptr i8, ptr %.pre.i.i.i58, i64 %.114.i.i.i62
  %336 = load i8, ptr %335, align 1
  store i8 %336, ptr %334, align 1
  %337 = add nuw nsw i64 %.114.i.i.i62, 1
  %338 = icmp slt i64 %337, %315
  br i1 %338, label %.lr.ph15.i.i.i61, label %.preheader.i.i.i59, !llvm.loop !47

rl_m_append__String_int8_t.exit66:                ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i64, %.preheader.i.i.i59
  %339 = phi ptr [ %.pre2.i.i65, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i64 ], [ %321, %.preheader.i.i.i59 ]
  %340 = getelementptr i8, ptr %339, i64 %315
  store i8 0, ptr %340, align 1
  %341 = load i64, ptr %5, align 8
  %342 = add i64 %341, 1
  store i64 %342, ptr %5, align 8
  br label %rl__indent_string__String_int64_t.exit94

rl_m_get__String_int64_t_r_int8_tRef.exit67:      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %343 = load i64, ptr %5, align 8
  %344 = icmp sgt i64 %343, 0
  br i1 %344, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68, label %345

345:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit67
  %346 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68:   ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit67
  %347 = load ptr, ptr %4, align 8
  %348 = getelementptr i8, ptr %347, i64 %343
  %349 = getelementptr i8, ptr %348, i64 -1
  store i8 %57, ptr %349, align 1
  %350 = load i64, ptr %5, align 8
  %351 = add i64 %350, 1
  %352 = load i64, ptr %6, align 8
  %353 = icmp sgt i64 %352, %351
  br i1 %353, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i76, label %354

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i76: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68
  %.pre2.i.i77 = load ptr, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit78

354:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68
  %355 = shl i64 %351, 1
  %356 = tail call ptr @malloc(i64 %355)
  %357 = ptrtoint ptr %356 to i64
  %358 = icmp sgt i64 %355, 0
  br i1 %358, label %.lr.ph.preheader.i.i.i75, label %.preheader12.i.i.i69

.lr.ph.preheader.i.i.i75:                         ; preds = %354
  tail call void @llvm.memset.p0.i64(ptr align 1 %356, i8 0, i64 %355, i1 false)
  br label %.preheader12.i.i.i69

.preheader12.i.i.i69:                             ; preds = %.lr.ph.preheader.i.i.i75, %354
  %359 = icmp sgt i64 %350, 0
  %.pre.i.i.i70 = load ptr, ptr %4, align 8
  br i1 %359, label %iter.check407, label %.preheader.i.i.i71

iter.check407:                                    ; preds = %.preheader12.i.i.i69
  %.pre.i.i.i70402 = ptrtoint ptr %.pre.i.i.i70 to i64
  %min.iters.check405 = icmp ult i64 %350, 8
  %360 = sub i64 %357, %.pre.i.i.i70402
  %diff.check403 = icmp ult i64 %360, 32
  %or.cond706 = select i1 %min.iters.check405, i1 true, i1 %diff.check403
  br i1 %or.cond706, label %.lr.ph15.i.i.i73.preheader, label %vector.main.loop.iter.check409

vector.main.loop.iter.check409:                   ; preds = %iter.check407
  %min.iters.check408 = icmp ult i64 %350, 32
  br i1 %min.iters.check408, label %vec.epilog.ph421, label %vector.ph410

vector.ph410:                                     ; preds = %vector.main.loop.iter.check409
  %n.vec412 = and i64 %350, 9223372036854775776
  br label %vector.body413

vector.body413:                                   ; preds = %vector.body413, %vector.ph410
  %index414 = phi i64 [ 0, %vector.ph410 ], [ %index.next417, %vector.body413 ]
  %361 = getelementptr i8, ptr %356, i64 %index414
  %362 = getelementptr i8, ptr %.pre.i.i.i70, i64 %index414
  %363 = getelementptr i8, ptr %362, i64 16
  %wide.load415 = load <16 x i8>, ptr %362, align 1
  %wide.load416 = load <16 x i8>, ptr %363, align 1
  %364 = getelementptr i8, ptr %361, i64 16
  store <16 x i8> %wide.load415, ptr %361, align 1
  store <16 x i8> %wide.load416, ptr %364, align 1
  %index.next417 = add nuw i64 %index414, 32
  %365 = icmp eq i64 %index.next417, %n.vec412
  br i1 %365, label %middle.block404, label %vector.body413, !llvm.loop !48

middle.block404:                                  ; preds = %vector.body413
  %cmp.n418 = icmp eq i64 %350, %n.vec412
  br i1 %cmp.n418, label %.preheader.i.i.i71, label %vec.epilog.iter.check422

vec.epilog.iter.check422:                         ; preds = %middle.block404
  %n.vec.remaining423 = and i64 %350, 24
  %min.epilog.iters.check424 = icmp eq i64 %n.vec.remaining423, 0
  br i1 %min.epilog.iters.check424, label %.lr.ph15.i.i.i73.preheader, label %vec.epilog.ph421

.lr.ph15.i.i.i73.preheader:                       ; preds = %vec.epilog.middle.block419, %iter.check407, %vec.epilog.iter.check422
  %.114.i.i.i74.ph = phi i64 [ 0, %iter.check407 ], [ %n.vec412, %vec.epilog.iter.check422 ], [ %n.vec427, %vec.epilog.middle.block419 ]
  br label %.lr.ph15.i.i.i73

vec.epilog.ph421:                                 ; preds = %vector.main.loop.iter.check409, %vec.epilog.iter.check422
  %vec.epilog.resume.val425 = phi i64 [ %n.vec412, %vec.epilog.iter.check422 ], [ 0, %vector.main.loop.iter.check409 ]
  %n.vec427 = and i64 %350, 9223372036854775800
  br label %vec.epilog.vector.body429

vec.epilog.vector.body429:                        ; preds = %vec.epilog.vector.body429, %vec.epilog.ph421
  %index430 = phi i64 [ %vec.epilog.resume.val425, %vec.epilog.ph421 ], [ %index.next432, %vec.epilog.vector.body429 ]
  %366 = getelementptr i8, ptr %356, i64 %index430
  %367 = getelementptr i8, ptr %.pre.i.i.i70, i64 %index430
  %wide.load431 = load <8 x i8>, ptr %367, align 1
  store <8 x i8> %wide.load431, ptr %366, align 1
  %index.next432 = add nuw i64 %index430, 8
  %368 = icmp eq i64 %index.next432, %n.vec427
  br i1 %368, label %vec.epilog.middle.block419, label %vec.epilog.vector.body429, !llvm.loop !49

vec.epilog.middle.block419:                       ; preds = %vec.epilog.vector.body429
  %cmp.n433 = icmp eq i64 %350, %n.vec427
  br i1 %cmp.n433, label %.preheader.i.i.i71, label %.lr.ph15.i.i.i73.preheader

.preheader.i.i.i71:                               ; preds = %.lr.ph15.i.i.i73, %middle.block404, %vec.epilog.middle.block419, %.preheader12.i.i.i69
  tail call void @free(ptr %.pre.i.i.i70)
  store i64 %355, ptr %6, align 8
  store ptr %356, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit78

.lr.ph15.i.i.i73:                                 ; preds = %.lr.ph15.i.i.i73.preheader, %.lr.ph15.i.i.i73
  %.114.i.i.i74 = phi i64 [ %372, %.lr.ph15.i.i.i73 ], [ %.114.i.i.i74.ph, %.lr.ph15.i.i.i73.preheader ]
  %369 = getelementptr i8, ptr %356, i64 %.114.i.i.i74
  %370 = getelementptr i8, ptr %.pre.i.i.i70, i64 %.114.i.i.i74
  %371 = load i8, ptr %370, align 1
  store i8 %371, ptr %369, align 1
  %372 = add nuw nsw i64 %.114.i.i.i74, 1
  %373 = icmp slt i64 %372, %350
  br i1 %373, label %.lr.ph15.i.i.i73, label %.preheader.i.i.i71, !llvm.loop !50

rl_m_append__String_int8_t.exit78:                ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i76, %.preheader.i.i.i71
  %374 = phi ptr [ %.pre2.i.i77, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i76 ], [ %356, %.preheader.i.i.i71 ]
  %375 = getelementptr i8, ptr %374, i64 %350
  store i8 0, ptr %375, align 1
  %376 = load i64, ptr %5, align 8
  %377 = add i64 %376, 1
  store i64 %377, ptr %5, align 8
  %378 = icmp ult i64 %376, 9223372036854775807
  br i1 %378, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79, label %379

379:                                              ; preds = %rl_m_append__String_int8_t.exit78
  %380 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79:   ; preds = %rl_m_append__String_int8_t.exit78
  %381 = load ptr, ptr %4, align 8
  %382 = getelementptr i8, ptr %381, i64 %377
  %383 = getelementptr i8, ptr %382, i64 -1
  store i8 10, ptr %383, align 1
  %384 = load i64, ptr %5, align 8
  %385 = add i64 %384, 1
  %386 = load i64, ptr %6, align 8
  %387 = icmp sgt i64 %386, %385
  br i1 %387, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i87, label %388

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i87: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79
  %.pre2.i.i88 = load ptr, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit89

388:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79
  %389 = shl i64 %385, 1
  %390 = tail call ptr @malloc(i64 %389)
  %391 = ptrtoint ptr %390 to i64
  %392 = icmp sgt i64 %389, 0
  br i1 %392, label %.lr.ph.preheader.i.i.i86, label %.preheader12.i.i.i80

.lr.ph.preheader.i.i.i86:                         ; preds = %388
  tail call void @llvm.memset.p0.i64(ptr align 1 %390, i8 0, i64 %389, i1 false)
  br label %.preheader12.i.i.i80

.preheader12.i.i.i80:                             ; preds = %.lr.ph.preheader.i.i.i86, %388
  %393 = icmp sgt i64 %384, 0
  %.pre.i.i.i81 = load ptr, ptr %4, align 8
  br i1 %393, label %iter.check374, label %.preheader.i.i.i82

iter.check374:                                    ; preds = %.preheader12.i.i.i80
  %.pre.i.i.i81369 = ptrtoint ptr %.pre.i.i.i81 to i64
  %min.iters.check372 = icmp ult i64 %384, 8
  %394 = sub i64 %391, %.pre.i.i.i81369
  %diff.check370 = icmp ult i64 %394, 32
  %or.cond707 = select i1 %min.iters.check372, i1 true, i1 %diff.check370
  br i1 %or.cond707, label %.lr.ph15.i.i.i84.preheader, label %vector.main.loop.iter.check376

vector.main.loop.iter.check376:                   ; preds = %iter.check374
  %min.iters.check375 = icmp ult i64 %384, 32
  br i1 %min.iters.check375, label %vec.epilog.ph388, label %vector.ph377

vector.ph377:                                     ; preds = %vector.main.loop.iter.check376
  %n.vec379 = and i64 %384, 9223372036854775776
  br label %vector.body380

vector.body380:                                   ; preds = %vector.body380, %vector.ph377
  %index381 = phi i64 [ 0, %vector.ph377 ], [ %index.next384, %vector.body380 ]
  %395 = getelementptr i8, ptr %390, i64 %index381
  %396 = getelementptr i8, ptr %.pre.i.i.i81, i64 %index381
  %397 = getelementptr i8, ptr %396, i64 16
  %wide.load382 = load <16 x i8>, ptr %396, align 1
  %wide.load383 = load <16 x i8>, ptr %397, align 1
  %398 = getelementptr i8, ptr %395, i64 16
  store <16 x i8> %wide.load382, ptr %395, align 1
  store <16 x i8> %wide.load383, ptr %398, align 1
  %index.next384 = add nuw i64 %index381, 32
  %399 = icmp eq i64 %index.next384, %n.vec379
  br i1 %399, label %middle.block371, label %vector.body380, !llvm.loop !51

middle.block371:                                  ; preds = %vector.body380
  %cmp.n385 = icmp eq i64 %384, %n.vec379
  br i1 %cmp.n385, label %.preheader.i.i.i82, label %vec.epilog.iter.check389

vec.epilog.iter.check389:                         ; preds = %middle.block371
  %n.vec.remaining390 = and i64 %384, 24
  %min.epilog.iters.check391 = icmp eq i64 %n.vec.remaining390, 0
  br i1 %min.epilog.iters.check391, label %.lr.ph15.i.i.i84.preheader, label %vec.epilog.ph388

.lr.ph15.i.i.i84.preheader:                       ; preds = %vec.epilog.middle.block386, %iter.check374, %vec.epilog.iter.check389
  %.114.i.i.i85.ph = phi i64 [ 0, %iter.check374 ], [ %n.vec379, %vec.epilog.iter.check389 ], [ %n.vec394, %vec.epilog.middle.block386 ]
  br label %.lr.ph15.i.i.i84

vec.epilog.ph388:                                 ; preds = %vector.main.loop.iter.check376, %vec.epilog.iter.check389
  %vec.epilog.resume.val392 = phi i64 [ %n.vec379, %vec.epilog.iter.check389 ], [ 0, %vector.main.loop.iter.check376 ]
  %n.vec394 = and i64 %384, 9223372036854775800
  br label %vec.epilog.vector.body396

vec.epilog.vector.body396:                        ; preds = %vec.epilog.vector.body396, %vec.epilog.ph388
  %index397 = phi i64 [ %vec.epilog.resume.val392, %vec.epilog.ph388 ], [ %index.next399, %vec.epilog.vector.body396 ]
  %400 = getelementptr i8, ptr %390, i64 %index397
  %401 = getelementptr i8, ptr %.pre.i.i.i81, i64 %index397
  %wide.load398 = load <8 x i8>, ptr %401, align 1
  store <8 x i8> %wide.load398, ptr %400, align 1
  %index.next399 = add nuw i64 %index397, 8
  %402 = icmp eq i64 %index.next399, %n.vec394
  br i1 %402, label %vec.epilog.middle.block386, label %vec.epilog.vector.body396, !llvm.loop !52

vec.epilog.middle.block386:                       ; preds = %vec.epilog.vector.body396
  %cmp.n400 = icmp eq i64 %384, %n.vec394
  br i1 %cmp.n400, label %.preheader.i.i.i82, label %.lr.ph15.i.i.i84.preheader

.preheader.i.i.i82:                               ; preds = %.lr.ph15.i.i.i84, %middle.block371, %vec.epilog.middle.block386, %.preheader12.i.i.i80
  tail call void @free(ptr %.pre.i.i.i81)
  store i64 %389, ptr %6, align 8
  store ptr %390, ptr %4, align 8
  br label %rl_m_append__String_int8_t.exit89

.lr.ph15.i.i.i84:                                 ; preds = %.lr.ph15.i.i.i84.preheader, %.lr.ph15.i.i.i84
  %.114.i.i.i85 = phi i64 [ %406, %.lr.ph15.i.i.i84 ], [ %.114.i.i.i85.ph, %.lr.ph15.i.i.i84.preheader ]
  %403 = getelementptr i8, ptr %390, i64 %.114.i.i.i85
  %404 = getelementptr i8, ptr %.pre.i.i.i81, i64 %.114.i.i.i85
  %405 = load i8, ptr %404, align 1
  store i8 %405, ptr %403, align 1
  %406 = add nuw nsw i64 %.114.i.i.i85, 1
  %407 = icmp slt i64 %406, %384
  br i1 %407, label %.lr.ph15.i.i.i84, label %.preheader.i.i.i82, !llvm.loop !53

rl_m_append__String_int8_t.exit89:                ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i87, %.preheader.i.i.i82
  %408 = phi ptr [ %.pre2.i.i88, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i87 ], [ %390, %.preheader.i.i.i82 ]
  %409 = getelementptr i8, ptr %408, i64 %384
  store i8 0, ptr %409, align 1
  %410 = load i64, ptr %5, align 8
  %411 = add i64 %410, 1
  store i64 %411, ptr %5, align 8
  %412 = add i64 %.0204216, 1
  %.not2.i90 = icmp eq i64 %412, 0
  br i1 %.not2.i90, label %rl__indent_string__String_int64_t.exit94, label %.lr.ph.i91

.lr.ph.i91:                                       ; preds = %rl_m_append__String_int8_t.exit89, %rl_m_append__String_strlit.exit146
  %413 = phi i64 [ %500, %rl_m_append__String_strlit.exit146 ], [ %411, %rl_m_append__String_int8_t.exit89 ]
  %.03.i92 = phi i64 [ %501, %rl_m_append__String_strlit.exit146 ], [ 0, %rl_m_append__String_int8_t.exit89 ]
  %414 = icmp sgt i64 %413, 0
  br i1 %414, label %.lr.ph.i122, label %415

415:                                              ; preds = %.lr.ph.i91
  %416 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
  tail call void @llvm.trap()
  unreachable

.lr.ph.i122:                                      ; preds = %.lr.ph.i91
  %417 = add nsw i64 %413, -1
  %418 = load ptr, ptr %4, align 8
  %419 = getelementptr i8, ptr %418, i64 %417
  store i64 %417, ptr %5, align 8
  store i8 0, ptr %419, align 1
  %.pre16.i121 = load i64, ptr %5, align 8
  %420 = add i64 %.pre16.i121, 1
  %421 = load i64, ptr %6, align 8
  %422 = icmp sgt i64 %421, %420
  br i1 %422, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i144, label %423

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i144: ; preds = %.lr.ph.i122
  %.pre2.i.i145 = load ptr, ptr %4, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i129

423:                                              ; preds = %.lr.ph.i122
  %424 = shl i64 %420, 1
  %425 = tail call ptr @malloc(i64 %424)
  %426 = ptrtoint ptr %425 to i64
  %427 = icmp sgt i64 %424, 0
  br i1 %427, label %.lr.ph.preheader.i.i.i143, label %.preheader12.i.i.i124

.lr.ph.preheader.i.i.i143:                        ; preds = %423
  tail call void @llvm.memset.p0.i64(ptr align 1 %425, i8 0, i64 %424, i1 false)
  br label %.preheader12.i.i.i124

.preheader12.i.i.i124:                            ; preds = %.lr.ph.preheader.i.i.i143, %423
  %428 = icmp sgt i64 %.pre16.i121, 0
  %.pre.i.i.i125 = load ptr, ptr %4, align 8
  br i1 %428, label %iter.check341, label %.preheader.i.i.i126

iter.check341:                                    ; preds = %.preheader12.i.i.i124
  %.pre.i.i.i125336 = ptrtoint ptr %.pre.i.i.i125 to i64
  %min.iters.check339 = icmp ult i64 %.pre16.i121, 8
  %429 = sub i64 %426, %.pre.i.i.i125336
  %diff.check337 = icmp ult i64 %429, 32
  %or.cond708 = select i1 %min.iters.check339, i1 true, i1 %diff.check337
  br i1 %or.cond708, label %.lr.ph15.i.i.i141.preheader, label %vector.main.loop.iter.check343

vector.main.loop.iter.check343:                   ; preds = %iter.check341
  %min.iters.check342 = icmp ult i64 %.pre16.i121, 32
  br i1 %min.iters.check342, label %vec.epilog.ph355, label %vector.ph344

vector.ph344:                                     ; preds = %vector.main.loop.iter.check343
  %n.vec346 = and i64 %.pre16.i121, 9223372036854775776
  br label %vector.body347

vector.body347:                                   ; preds = %vector.body347, %vector.ph344
  %index348 = phi i64 [ 0, %vector.ph344 ], [ %index.next351, %vector.body347 ]
  %430 = getelementptr i8, ptr %425, i64 %index348
  %431 = getelementptr i8, ptr %.pre.i.i.i125, i64 %index348
  %432 = getelementptr i8, ptr %431, i64 16
  %wide.load349 = load <16 x i8>, ptr %431, align 1
  %wide.load350 = load <16 x i8>, ptr %432, align 1
  %433 = getelementptr i8, ptr %430, i64 16
  store <16 x i8> %wide.load349, ptr %430, align 1
  store <16 x i8> %wide.load350, ptr %433, align 1
  %index.next351 = add nuw i64 %index348, 32
  %434 = icmp eq i64 %index.next351, %n.vec346
  br i1 %434, label %middle.block338, label %vector.body347, !llvm.loop !54

middle.block338:                                  ; preds = %vector.body347
  %cmp.n352 = icmp eq i64 %.pre16.i121, %n.vec346
  br i1 %cmp.n352, label %.preheader.i.i.i126, label %vec.epilog.iter.check356

vec.epilog.iter.check356:                         ; preds = %middle.block338
  %n.vec.remaining357 = and i64 %.pre16.i121, 24
  %min.epilog.iters.check358 = icmp eq i64 %n.vec.remaining357, 0
  br i1 %min.epilog.iters.check358, label %.lr.ph15.i.i.i141.preheader, label %vec.epilog.ph355

.lr.ph15.i.i.i141.preheader:                      ; preds = %vec.epilog.middle.block353, %iter.check341, %vec.epilog.iter.check356
  %.114.i.i.i142.ph = phi i64 [ 0, %iter.check341 ], [ %n.vec346, %vec.epilog.iter.check356 ], [ %n.vec361, %vec.epilog.middle.block353 ]
  br label %.lr.ph15.i.i.i141

vec.epilog.ph355:                                 ; preds = %vector.main.loop.iter.check343, %vec.epilog.iter.check356
  %vec.epilog.resume.val359 = phi i64 [ %n.vec346, %vec.epilog.iter.check356 ], [ 0, %vector.main.loop.iter.check343 ]
  %n.vec361 = and i64 %.pre16.i121, 9223372036854775800
  br label %vec.epilog.vector.body363

vec.epilog.vector.body363:                        ; preds = %vec.epilog.vector.body363, %vec.epilog.ph355
  %index364 = phi i64 [ %vec.epilog.resume.val359, %vec.epilog.ph355 ], [ %index.next366, %vec.epilog.vector.body363 ]
  %435 = getelementptr i8, ptr %425, i64 %index364
  %436 = getelementptr i8, ptr %.pre.i.i.i125, i64 %index364
  %wide.load365 = load <8 x i8>, ptr %436, align 1
  store <8 x i8> %wide.load365, ptr %435, align 1
  %index.next366 = add nuw i64 %index364, 8
  %437 = icmp eq i64 %index.next366, %n.vec361
  br i1 %437, label %vec.epilog.middle.block353, label %vec.epilog.vector.body363, !llvm.loop !55

vec.epilog.middle.block353:                       ; preds = %vec.epilog.vector.body363
  %cmp.n367 = icmp eq i64 %.pre16.i121, %n.vec361
  br i1 %cmp.n367, label %.preheader.i.i.i126, label %.lr.ph15.i.i.i141.preheader

.preheader.i.i.i126:                              ; preds = %.lr.ph15.i.i.i141, %middle.block338, %vec.epilog.middle.block353, %.preheader12.i.i.i124
  tail call void @free(ptr %.pre.i.i.i125)
  store i64 %424, ptr %6, align 8
  store ptr %425, ptr %4, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i129

.lr.ph15.i.i.i141:                                ; preds = %.lr.ph15.i.i.i141.preheader, %.lr.ph15.i.i.i141
  %.114.i.i.i142 = phi i64 [ %441, %.lr.ph15.i.i.i141 ], [ %.114.i.i.i142.ph, %.lr.ph15.i.i.i141.preheader ]
  %438 = getelementptr i8, ptr %425, i64 %.114.i.i.i142
  %439 = getelementptr i8, ptr %.pre.i.i.i125, i64 %.114.i.i.i142
  %440 = load i8, ptr %439, align 1
  store i8 %440, ptr %438, align 1
  %441 = add nuw nsw i64 %.114.i.i.i142, 1
  %442 = icmp slt i64 %441, %.pre16.i121
  br i1 %442, label %.lr.ph15.i.i.i141, label %.preheader.i.i.i126, !llvm.loop !56

rl_m_append__VectorTint8_tT_int8_t.exit.i129:     ; preds = %.preheader.i.i.i126, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i144
  %443 = phi ptr [ %.pre2.i.i145, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i144 ], [ %425, %.preheader.i.i.i126 ]
  %444 = getelementptr i8, ptr %443, i64 %.pre16.i121
  store i8 32, ptr %444, align 1
  %445 = load i64, ptr %5, align 8
  %446 = add i64 %445, 1
  store i64 %446, ptr %5, align 8
  %447 = add i64 %445, 2
  %448 = load i64, ptr %6, align 8
  %449 = icmp sgt i64 %448, %447
  br i1 %449, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i144.1, label %450

450:                                              ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i129
  %451 = shl i64 %447, 1
  %452 = tail call ptr @malloc(i64 %451)
  %453 = ptrtoint ptr %452 to i64
  %454 = icmp sgt i64 %451, 0
  br i1 %454, label %.lr.ph.preheader.i.i.i143.1, label %.preheader12.i.i.i124.1

.lr.ph.preheader.i.i.i143.1:                      ; preds = %450
  tail call void @llvm.memset.p0.i64(ptr align 1 %452, i8 0, i64 %451, i1 false)
  br label %.preheader12.i.i.i124.1

.preheader12.i.i.i124.1:                          ; preds = %.lr.ph.preheader.i.i.i143.1, %450
  %455 = icmp ult i64 %445, 9223372036854775807
  %.pre.i.i.i125.1 = load ptr, ptr %4, align 8
  br i1 %455, label %iter.check308, label %.preheader.i.i.i126.1

iter.check308:                                    ; preds = %.preheader12.i.i.i124.1
  %.pre.i.i.i125.1303 = ptrtoint ptr %.pre.i.i.i125.1 to i64
  %min.iters.check306 = icmp ult i64 %446, 8
  %456 = sub i64 %453, %.pre.i.i.i125.1303
  %diff.check304 = icmp ult i64 %456, 32
  %or.cond709 = select i1 %min.iters.check306, i1 true, i1 %diff.check304
  br i1 %or.cond709, label %.lr.ph15.i.i.i141.1.preheader, label %vector.main.loop.iter.check310

vector.main.loop.iter.check310:                   ; preds = %iter.check308
  %min.iters.check309 = icmp ult i64 %446, 32
  br i1 %min.iters.check309, label %vec.epilog.ph322, label %vector.ph311

vector.ph311:                                     ; preds = %vector.main.loop.iter.check310
  %n.vec313 = and i64 %446, -32
  br label %vector.body314

vector.body314:                                   ; preds = %vector.body314, %vector.ph311
  %index315 = phi i64 [ 0, %vector.ph311 ], [ %index.next318, %vector.body314 ]
  %457 = getelementptr i8, ptr %452, i64 %index315
  %458 = getelementptr i8, ptr %.pre.i.i.i125.1, i64 %index315
  %459 = getelementptr i8, ptr %458, i64 16
  %wide.load316 = load <16 x i8>, ptr %458, align 1
  %wide.load317 = load <16 x i8>, ptr %459, align 1
  %460 = getelementptr i8, ptr %457, i64 16
  store <16 x i8> %wide.load316, ptr %457, align 1
  store <16 x i8> %wide.load317, ptr %460, align 1
  %index.next318 = add nuw i64 %index315, 32
  %461 = icmp eq i64 %index.next318, %n.vec313
  br i1 %461, label %middle.block305, label %vector.body314, !llvm.loop !57

middle.block305:                                  ; preds = %vector.body314
  %cmp.n319 = icmp eq i64 %446, %n.vec313
  br i1 %cmp.n319, label %.preheader.i.i.i126.1, label %vec.epilog.iter.check323

vec.epilog.iter.check323:                         ; preds = %middle.block305
  %n.vec.remaining324 = and i64 %446, 24
  %min.epilog.iters.check325 = icmp eq i64 %n.vec.remaining324, 0
  br i1 %min.epilog.iters.check325, label %.lr.ph15.i.i.i141.1.preheader, label %vec.epilog.ph322

vec.epilog.ph322:                                 ; preds = %vector.main.loop.iter.check310, %vec.epilog.iter.check323
  %vec.epilog.resume.val326 = phi i64 [ %n.vec313, %vec.epilog.iter.check323 ], [ 0, %vector.main.loop.iter.check310 ]
  %n.vec328 = and i64 %446, -8
  br label %vec.epilog.vector.body330

vec.epilog.vector.body330:                        ; preds = %vec.epilog.vector.body330, %vec.epilog.ph322
  %index331 = phi i64 [ %vec.epilog.resume.val326, %vec.epilog.ph322 ], [ %index.next333, %vec.epilog.vector.body330 ]
  %462 = getelementptr i8, ptr %452, i64 %index331
  %463 = getelementptr i8, ptr %.pre.i.i.i125.1, i64 %index331
  %wide.load332 = load <8 x i8>, ptr %463, align 1
  store <8 x i8> %wide.load332, ptr %462, align 1
  %index.next333 = add nuw i64 %index331, 8
  %464 = icmp eq i64 %index.next333, %n.vec328
  br i1 %464, label %vec.epilog.middle.block320, label %vec.epilog.vector.body330, !llvm.loop !58

vec.epilog.middle.block320:                       ; preds = %vec.epilog.vector.body330
  %cmp.n334 = icmp eq i64 %446, %n.vec328
  br i1 %cmp.n334, label %.preheader.i.i.i126.1, label %.lr.ph15.i.i.i141.1.preheader

.lr.ph15.i.i.i141.1.preheader:                    ; preds = %vec.epilog.middle.block320, %iter.check308, %vec.epilog.iter.check323
  %.114.i.i.i142.1.ph = phi i64 [ 0, %iter.check308 ], [ %n.vec313, %vec.epilog.iter.check323 ], [ %n.vec328, %vec.epilog.middle.block320 ]
  br label %.lr.ph15.i.i.i141.1

.lr.ph15.i.i.i141.1:                              ; preds = %.lr.ph15.i.i.i141.1.preheader, %.lr.ph15.i.i.i141.1
  %.114.i.i.i142.1 = phi i64 [ %468, %.lr.ph15.i.i.i141.1 ], [ %.114.i.i.i142.1.ph, %.lr.ph15.i.i.i141.1.preheader ]
  %465 = getelementptr i8, ptr %452, i64 %.114.i.i.i142.1
  %466 = getelementptr i8, ptr %.pre.i.i.i125.1, i64 %.114.i.i.i142.1
  %467 = load i8, ptr %466, align 1
  store i8 %467, ptr %465, align 1
  %468 = add nuw nsw i64 %.114.i.i.i142.1, 1
  %469 = icmp slt i64 %468, %446
  br i1 %469, label %.lr.ph15.i.i.i141.1, label %.preheader.i.i.i126.1, !llvm.loop !59

.preheader.i.i.i126.1:                            ; preds = %.lr.ph15.i.i.i141.1, %middle.block305, %vec.epilog.middle.block320, %.preheader12.i.i.i124.1
  tail call void @free(ptr %.pre.i.i.i125.1)
  store i64 %451, ptr %6, align 8
  store ptr %452, ptr %4, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i129.1

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i144.1: ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i129
  %.pre2.i.i145.1 = load ptr, ptr %4, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i129.1

rl_m_append__VectorTint8_tT_int8_t.exit.i129.1:   ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i144.1, %.preheader.i.i.i126.1
  %470 = phi ptr [ %.pre2.i.i145.1, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i144.1 ], [ %452, %.preheader.i.i.i126.1 ]
  %471 = getelementptr i8, ptr %470, i64 %446
  store i8 32, ptr %471, align 1
  %472 = load i64, ptr %5, align 8
  %473 = add i64 %472, 1
  store i64 %473, ptr %5, align 8
  %474 = add i64 %472, 2
  %475 = load i64, ptr %6, align 8
  %476 = icmp sgt i64 %475, %474
  br i1 %476, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10.i139, label %477

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10.i139: ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i129.1
  %.pre2.i11.i140 = load ptr, ptr %4, align 8
  br label %rl_m_append__String_strlit.exit146

477:                                              ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i129.1
  %478 = shl i64 %474, 1
  %479 = tail call ptr @malloc(i64 %478)
  %480 = ptrtoint ptr %479 to i64
  %481 = icmp sgt i64 %478, 0
  br i1 %481, label %.lr.ph.preheader.i.i9.i138, label %.preheader12.i.i3.i132

.lr.ph.preheader.i.i9.i138:                       ; preds = %477
  tail call void @llvm.memset.p0.i64(ptr align 1 %479, i8 0, i64 %478, i1 false)
  br label %.preheader12.i.i3.i132

.preheader12.i.i3.i132:                           ; preds = %.lr.ph.preheader.i.i9.i138, %477
  %482 = icmp ult i64 %472, 9223372036854775807
  %.pre.i.i4.i133 = load ptr, ptr %4, align 8
  br i1 %482, label %iter.check275, label %.preheader.i.i5.i134

iter.check275:                                    ; preds = %.preheader12.i.i3.i132
  %.pre.i.i4.i133270 = ptrtoint ptr %.pre.i.i4.i133 to i64
  %min.iters.check273 = icmp ult i64 %473, 8
  %483 = sub i64 %480, %.pre.i.i4.i133270
  %diff.check271 = icmp ult i64 %483, 32
  %or.cond710 = select i1 %min.iters.check273, i1 true, i1 %diff.check271
  br i1 %or.cond710, label %.lr.ph15.i.i7.i136.preheader, label %vector.main.loop.iter.check277

vector.main.loop.iter.check277:                   ; preds = %iter.check275
  %min.iters.check276 = icmp ult i64 %473, 32
  br i1 %min.iters.check276, label %vec.epilog.ph289, label %vector.ph278

vector.ph278:                                     ; preds = %vector.main.loop.iter.check277
  %n.vec280 = and i64 %473, -32
  br label %vector.body281

vector.body281:                                   ; preds = %vector.body281, %vector.ph278
  %index282 = phi i64 [ 0, %vector.ph278 ], [ %index.next285, %vector.body281 ]
  %484 = getelementptr i8, ptr %479, i64 %index282
  %485 = getelementptr i8, ptr %.pre.i.i4.i133, i64 %index282
  %486 = getelementptr i8, ptr %485, i64 16
  %wide.load283 = load <16 x i8>, ptr %485, align 1
  %wide.load284 = load <16 x i8>, ptr %486, align 1
  %487 = getelementptr i8, ptr %484, i64 16
  store <16 x i8> %wide.load283, ptr %484, align 1
  store <16 x i8> %wide.load284, ptr %487, align 1
  %index.next285 = add nuw i64 %index282, 32
  %488 = icmp eq i64 %index.next285, %n.vec280
  br i1 %488, label %middle.block272, label %vector.body281, !llvm.loop !60

middle.block272:                                  ; preds = %vector.body281
  %cmp.n286 = icmp eq i64 %473, %n.vec280
  br i1 %cmp.n286, label %.preheader.i.i5.i134, label %vec.epilog.iter.check290

vec.epilog.iter.check290:                         ; preds = %middle.block272
  %n.vec.remaining291 = and i64 %473, 24
  %min.epilog.iters.check292 = icmp eq i64 %n.vec.remaining291, 0
  br i1 %min.epilog.iters.check292, label %.lr.ph15.i.i7.i136.preheader, label %vec.epilog.ph289

.lr.ph15.i.i7.i136.preheader:                     ; preds = %vec.epilog.middle.block287, %iter.check275, %vec.epilog.iter.check290
  %.114.i.i8.i137.ph = phi i64 [ 0, %iter.check275 ], [ %n.vec280, %vec.epilog.iter.check290 ], [ %n.vec295, %vec.epilog.middle.block287 ]
  br label %.lr.ph15.i.i7.i136

vec.epilog.ph289:                                 ; preds = %vector.main.loop.iter.check277, %vec.epilog.iter.check290
  %vec.epilog.resume.val293 = phi i64 [ %n.vec280, %vec.epilog.iter.check290 ], [ 0, %vector.main.loop.iter.check277 ]
  %n.vec295 = and i64 %473, -8
  br label %vec.epilog.vector.body297

vec.epilog.vector.body297:                        ; preds = %vec.epilog.vector.body297, %vec.epilog.ph289
  %index298 = phi i64 [ %vec.epilog.resume.val293, %vec.epilog.ph289 ], [ %index.next300, %vec.epilog.vector.body297 ]
  %489 = getelementptr i8, ptr %479, i64 %index298
  %490 = getelementptr i8, ptr %.pre.i.i4.i133, i64 %index298
  %wide.load299 = load <8 x i8>, ptr %490, align 1
  store <8 x i8> %wide.load299, ptr %489, align 1
  %index.next300 = add nuw i64 %index298, 8
  %491 = icmp eq i64 %index.next300, %n.vec295
  br i1 %491, label %vec.epilog.middle.block287, label %vec.epilog.vector.body297, !llvm.loop !61

vec.epilog.middle.block287:                       ; preds = %vec.epilog.vector.body297
  %cmp.n301 = icmp eq i64 %473, %n.vec295
  br i1 %cmp.n301, label %.preheader.i.i5.i134, label %.lr.ph15.i.i7.i136.preheader

.preheader.i.i5.i134:                             ; preds = %.lr.ph15.i.i7.i136, %middle.block272, %vec.epilog.middle.block287, %.preheader12.i.i3.i132
  tail call void @free(ptr %.pre.i.i4.i133)
  store i64 %478, ptr %6, align 8
  store ptr %479, ptr %4, align 8
  br label %rl_m_append__String_strlit.exit146

.lr.ph15.i.i7.i136:                               ; preds = %.lr.ph15.i.i7.i136.preheader, %.lr.ph15.i.i7.i136
  %.114.i.i8.i137 = phi i64 [ %495, %.lr.ph15.i.i7.i136 ], [ %.114.i.i8.i137.ph, %.lr.ph15.i.i7.i136.preheader ]
  %492 = getelementptr i8, ptr %479, i64 %.114.i.i8.i137
  %493 = getelementptr i8, ptr %.pre.i.i4.i133, i64 %.114.i.i8.i137
  %494 = load i8, ptr %493, align 1
  store i8 %494, ptr %492, align 1
  %495 = add nuw nsw i64 %.114.i.i8.i137, 1
  %496 = icmp slt i64 %495, %473
  br i1 %496, label %.lr.ph15.i.i7.i136, label %.preheader.i.i5.i134, !llvm.loop !62

rl_m_append__String_strlit.exit146:               ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10.i139, %.preheader.i.i5.i134
  %497 = phi ptr [ %.pre2.i11.i140, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10.i139 ], [ %479, %.preheader.i.i5.i134 ]
  %498 = getelementptr i8, ptr %497, i64 %473
  store i8 0, ptr %498, align 1
  %499 = load i64, ptr %5, align 8
  %500 = add i64 %499, 1
  store i64 %500, ptr %5, align 8
  %501 = add nuw i64 %.03.i92, 1
  %.not.i93 = icmp eq i64 %.03.i92, %.0204216
  br i1 %.not.i93, label %rl__indent_string__String_int64_t.exit94, label %.lr.ph.i91

rl__indent_string__String_int64_t.exit94:         ; preds = %rl_m_append__String_strlit.exit146, %rl_m_get__String_int64_t_r_int8_tRef.exit38, %rl_m_append__String_int8_t.exit89, %rl_m_append__String_int8_t.exit66, %rl_m_append__String_int8_t.exit
  %.1205 = phi i64 [ %.0204216, %rl_m_append__String_int8_t.exit ], [ %211, %rl_m_append__String_int8_t.exit66 ], [ 0, %rl_m_append__String_int8_t.exit89 ], [ %.0204216, %rl_m_get__String_int64_t_r_int8_tRef.exit38 ], [ %412, %rl_m_append__String_strlit.exit146 ]
  %.1 = phi i64 [ %.0217, %rl_m_append__String_int8_t.exit ], [ %.0217, %rl_m_append__String_int8_t.exit66 ], [ %.0217, %rl_m_append__String_int8_t.exit89 ], [ %spec.select, %rl_m_get__String_int64_t_r_int8_tRef.exit38 ], [ %.0217, %rl_m_append__String_strlit.exit146 ]
  %502 = add i64 %.1, 1
  %503 = load i64, ptr %40, align 8
  %504 = add i64 %503, -1
  %505 = icmp slt i64 %502, %504
  br i1 %505, label %.lr.ph, label %.lr.ph.i.i95.preheader.loopexit

rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i: ; preds = %.lr.ph.i.i95.preheader, %rl_m_append__VectorTint8_tT_int8_t.exit.i154
  %.sroa.0192.1 = phi ptr [ %.sroa.0192.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i154 ], [ %45, %.lr.ph.i.i95.preheader ]
  %.sroa.12.0 = phi i64 [ %508, %rl_m_append__VectorTint8_tT_int8_t.exit.i154 ], [ 0, %.lr.ph.i.i95.preheader ]
  %.sroa.22.1 = phi i64 [ %.sroa.22.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i154 ], [ 4, %.lr.ph.i.i95.preheader ]
  %.sroa.0192.1666 = ptrtoint ptr %.sroa.0192.1 to i64
  %506 = load ptr, ptr %4, align 8
  %507 = getelementptr i8, ptr %506, i64 %.sroa.12.0
  %508 = add nuw nsw i64 %.sroa.12.0, 1
  %509 = icmp sgt i64 %.sroa.22.1, %508
  br i1 %509, label %rl_m_append__VectorTint8_tT_int8_t.exit.i154, label %510

510:                                              ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i
  %511 = shl nuw i64 %508, 1
  %512 = tail call ptr @malloc(i64 %511)
  %513 = ptrtoint ptr %512 to i64
  %514 = icmp sgt i64 %511, 0
  br i1 %514, label %.lr.ph.preheader.i.i.i157, label %.preheader12.i.i.i150

.lr.ph.preheader.i.i.i157:                        ; preds = %510
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(1) %512, i8 0, i64 %511, i1 false)
  br label %.preheader12.i.i.i150

.preheader12.i.i.i150:                            ; preds = %.lr.ph.preheader.i.i.i157, %510
  %.not = icmp eq i64 %.sroa.12.0, 0
  br i1 %.not, label %.preheader.i.i.i152, label %iter.check671

iter.check671:                                    ; preds = %.preheader12.i.i.i150
  %min.iters.check669 = icmp ult i64 %.sroa.12.0, 8
  %515 = sub i64 %513, %.sroa.0192.1666
  %diff.check667 = icmp ult i64 %515, 32
  %or.cond711 = or i1 %min.iters.check669, %diff.check667
  br i1 %or.cond711, label %.lr.ph15.i.i.i155.preheader, label %vector.main.loop.iter.check673

vector.main.loop.iter.check673:                   ; preds = %iter.check671
  %min.iters.check672 = icmp ult i64 %.sroa.12.0, 32
  br i1 %min.iters.check672, label %vec.epilog.ph685, label %vector.ph674

vector.ph674:                                     ; preds = %vector.main.loop.iter.check673
  %n.vec676 = and i64 %.sroa.12.0, 9223372036854775776
  br label %vector.body677

vector.body677:                                   ; preds = %vector.body677, %vector.ph674
  %index678 = phi i64 [ 0, %vector.ph674 ], [ %index.next681, %vector.body677 ]
  %516 = getelementptr i8, ptr %512, i64 %index678
  %517 = getelementptr i8, ptr %.sroa.0192.1, i64 %index678
  %518 = getelementptr i8, ptr %517, i64 16
  %wide.load679 = load <16 x i8>, ptr %517, align 1
  %wide.load680 = load <16 x i8>, ptr %518, align 1
  %519 = getelementptr i8, ptr %516, i64 16
  store <16 x i8> %wide.load679, ptr %516, align 1
  store <16 x i8> %wide.load680, ptr %519, align 1
  %index.next681 = add nuw i64 %index678, 32
  %520 = icmp eq i64 %index.next681, %n.vec676
  br i1 %520, label %middle.block668, label %vector.body677, !llvm.loop !63

middle.block668:                                  ; preds = %vector.body677
  %cmp.n682 = icmp eq i64 %.sroa.12.0, %n.vec676
  br i1 %cmp.n682, label %.preheader.i.i.i152, label %vec.epilog.iter.check686

vec.epilog.iter.check686:                         ; preds = %middle.block668
  %n.vec.remaining687 = and i64 %.sroa.12.0, 24
  %min.epilog.iters.check688 = icmp eq i64 %n.vec.remaining687, 0
  br i1 %min.epilog.iters.check688, label %.lr.ph15.i.i.i155.preheader, label %vec.epilog.ph685

.lr.ph15.i.i.i155.preheader:                      ; preds = %vec.epilog.middle.block683, %iter.check671, %vec.epilog.iter.check686
  %.114.i.i.i156.ph = phi i64 [ 0, %iter.check671 ], [ %n.vec676, %vec.epilog.iter.check686 ], [ %n.vec691, %vec.epilog.middle.block683 ]
  br label %.lr.ph15.i.i.i155

vec.epilog.ph685:                                 ; preds = %vector.main.loop.iter.check673, %vec.epilog.iter.check686
  %vec.epilog.resume.val689 = phi i64 [ %n.vec676, %vec.epilog.iter.check686 ], [ 0, %vector.main.loop.iter.check673 ]
  %n.vec691 = and i64 %.sroa.12.0, 9223372036854775800
  br label %vec.epilog.vector.body693

vec.epilog.vector.body693:                        ; preds = %vec.epilog.vector.body693, %vec.epilog.ph685
  %index694 = phi i64 [ %vec.epilog.resume.val689, %vec.epilog.ph685 ], [ %index.next696, %vec.epilog.vector.body693 ]
  %521 = getelementptr i8, ptr %512, i64 %index694
  %522 = getelementptr i8, ptr %.sroa.0192.1, i64 %index694
  %wide.load695 = load <8 x i8>, ptr %522, align 1
  store <8 x i8> %wide.load695, ptr %521, align 1
  %index.next696 = add nuw i64 %index694, 8
  %523 = icmp eq i64 %index.next696, %n.vec691
  br i1 %523, label %vec.epilog.middle.block683, label %vec.epilog.vector.body693, !llvm.loop !64

vec.epilog.middle.block683:                       ; preds = %vec.epilog.vector.body693
  %cmp.n697 = icmp eq i64 %.sroa.12.0, %n.vec691
  br i1 %cmp.n697, label %.preheader.i.i.i152, label %.lr.ph15.i.i.i155.preheader

.preheader.i.i.i152:                              ; preds = %.lr.ph15.i.i.i155, %middle.block668, %vec.epilog.middle.block683, %.preheader12.i.i.i150
  tail call void @free(ptr nonnull %.sroa.0192.1)
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i154

.lr.ph15.i.i.i155:                                ; preds = %.lr.ph15.i.i.i155.preheader, %.lr.ph15.i.i.i155
  %.114.i.i.i156 = phi i64 [ %527, %.lr.ph15.i.i.i155 ], [ %.114.i.i.i156.ph, %.lr.ph15.i.i.i155.preheader ]
  %524 = getelementptr i8, ptr %512, i64 %.114.i.i.i156
  %525 = getelementptr i8, ptr %.sroa.0192.1, i64 %.114.i.i.i156
  %526 = load i8, ptr %525, align 1
  store i8 %526, ptr %524, align 1
  %527 = add nuw nsw i64 %.114.i.i.i156, 1
  %528 = icmp ult i64 %527, %.sroa.12.0
  br i1 %528, label %.lr.ph15.i.i.i155, label %.preheader.i.i.i152, !llvm.loop !65

rl_m_append__VectorTint8_tT_int8_t.exit.i154:     ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, %.preheader.i.i.i152
  %.sroa.0192.2 = phi ptr [ %512, %.preheader.i.i.i152 ], [ %.sroa.0192.1, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i ]
  %.sroa.22.2 = phi i64 [ %511, %.preheader.i.i.i152 ], [ %.sroa.22.1, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i ]
  %529 = getelementptr i8, ptr %.sroa.0192.2, i64 %.sroa.12.0
  %530 = load i8, ptr %507, align 1
  store i8 %530, ptr %529, align 1
  %531 = load i64, ptr %5, align 8
  %532 = icmp slt i64 %508, %531
  br i1 %532, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, label %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit

rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit:  ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i154, %.lr.ph.i.i95.preheader
  %.sroa.0192.3 = phi ptr [ %45, %.lr.ph.i.i95.preheader ], [ %.sroa.0192.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i154 ]
  %.sroa.12.1 = phi i64 [ 0, %.lr.ph.i.i95.preheader ], [ %508, %rl_m_append__VectorTint8_tT_int8_t.exit.i154 ]
  %.sroa.22.3 = phi i64 [ 4, %.lr.ph.i.i95.preheader ], [ %.sroa.22.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i154 ]
  %533 = load i64, ptr %6, align 8
  %.not3.i.i = icmp eq i64 %533, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %534

534:                                              ; preds = %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit
  %535 = load ptr, ptr %4, align 8
  tail call void @free(ptr %535)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit, %534
  store ptr %.sroa.0192.3, ptr %0, align 1
  %.sroa.0.sroa.2.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 8
  store i64 %.sroa.12.1, ptr %.sroa.0.sroa.2.0..sroa_idx, align 1
  %.sroa.0.sroa.3.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 16
  store i64 %.sroa.22.3, ptr %.sroa.0.sroa.3.0..sroa_idx, align 1
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
  %8 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

9:                                                ; preds = %.lr.ph
  %10 = load ptr, ptr %0, align 8
  %11 = getelementptr i8, ptr %10, i64 %.015
  %12 = load i8, ptr %11, align 1
  %13 = icmp ult i64 %storemerge16, %5
  br i1 %13, label %16, label %14

14:                                               ; preds = %9
  %15 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %22 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %8 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

9:                                                ; preds = %2
  %10 = icmp sgt i64 %4, -9223372036854775807
  br i1 %10, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit, label %11

11:                                               ; preds = %9
  %12 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %.04.i = phi i64 [ %10, %.lr.ph.i ], [ %6, %2 ]
  %8 = load ptr, ptr %0, align 8
  %9 = getelementptr i8, ptr %8, i64 %.04.i
  store i8 0, ptr %9, align 1
  %10 = add nsw i64 %.04.i, 1
  %11 = load i64, ptr %3, align 8
  %12 = icmp slt i64 %10, %11
  br i1 %12, label %.lr.ph.i, label %._crit_edge.loopexit.i

._crit_edge.loopexit.i:                           ; preds = %.lr.ph.i
  %.pre.i = load i64, ptr %1, align 8
  %.pre6.i = sub i64 %11, %.pre.i
  br label %rl_m_drop_back__VectorTint8_tT_int64_t.exit

rl_m_drop_back__VectorTint8_tT_int64_t.exit:      ; preds = %2, %._crit_edge.loopexit.i
  %.pre-phi.i = phi i64 [ %.pre6.i, %._crit_edge.loopexit.i ], [ %6, %2 ]
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
  %10 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %17 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  br i1 %or.cond.not, label %rl_m_equal__String_String_r_bool.exit, label %.lr.ph.i, !llvm.loop !66

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
  %15 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %10 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %4 = getelementptr inbounds i8, ptr %3, i64 8
  %5 = getelementptr inbounds i8, ptr %3, i64 16
  store i64 4, ptr %5, align 8
  %6 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %6, ptr %3, align 8
  store i32 0, ptr %6, align 1
  store i64 1, ptr %4, align 8
  call void @rl_m_append__String_String(ptr nonnull %3, ptr %1)
  call void @rl_m_append__String_String(ptr nonnull %3, ptr %2)
  %7 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store i32 0, ptr %7, align 1
  %8 = load i64, ptr %4, align 8
  %9 = icmp sgt i64 %8, 0
  br i1 %9, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, label %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit

rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i: ; preds = %rl_m_init__String.exit, %rl_m_append__VectorTint8_tT_int8_t.exit.i
  %.sroa.22.1 = phi i64 [ %.sroa.22.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ], [ 4, %rl_m_init__String.exit ]
  %.sroa.12.0 = phi i64 [ %12, %rl_m_append__VectorTint8_tT_int8_t.exit.i ], [ 0, %rl_m_init__String.exit ]
  %.sroa.026.1 = phi ptr [ %.sroa.026.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ], [ %7, %rl_m_init__String.exit ]
  %.sroa.026.134 = ptrtoint ptr %.sroa.026.1 to i64
  %10 = load ptr, ptr %3, align 8
  %11 = getelementptr i8, ptr %10, i64 %.sroa.12.0
  %12 = add nuw nsw i64 %.sroa.12.0, 1
  %13 = icmp sgt i64 %.sroa.22.1, %12
  br i1 %13, label %rl_m_append__VectorTint8_tT_int8_t.exit.i, label %14

14:                                               ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i
  %15 = shl nuw i64 %12, 1
  %16 = tail call ptr @malloc(i64 %15)
  %17 = ptrtoint ptr %16 to i64
  %18 = icmp sgt i64 %15, 0
  br i1 %18, label %.lr.ph.preheader.i.i.i23, label %.preheader12.i.i.i17

.lr.ph.preheader.i.i.i23:                         ; preds = %14
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(1) %16, i8 0, i64 %15, i1 false)
  br label %.preheader12.i.i.i17

.preheader12.i.i.i17:                             ; preds = %.lr.ph.preheader.i.i.i23, %14
  %.not = icmp eq i64 %.sroa.12.0, 0
  br i1 %.not, label %.preheader.i.i.i19, label %iter.check

iter.check:                                       ; preds = %.preheader12.i.i.i17
  %min.iters.check = icmp ult i64 %.sroa.12.0, 8
  %19 = sub i64 %17, %.sroa.026.134
  %diff.check = icmp ult i64 %19, 32
  %or.cond = or i1 %min.iters.check, %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.i21.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check35 = icmp ult i64 %.sroa.12.0, 32
  br i1 %min.iters.check35, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %.sroa.12.0, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %20 = getelementptr i8, ptr %16, i64 %index
  %21 = getelementptr i8, ptr %.sroa.026.1, i64 %index
  %22 = getelementptr i8, ptr %21, i64 16
  %wide.load = load <16 x i8>, ptr %21, align 1
  %wide.load36 = load <16 x i8>, ptr %22, align 1
  %23 = getelementptr i8, ptr %20, i64 16
  store <16 x i8> %wide.load, ptr %20, align 1
  store <16 x i8> %wide.load36, ptr %23, align 1
  %index.next = add nuw i64 %index, 32
  %24 = icmp eq i64 %index.next, %n.vec
  br i1 %24, label %middle.block, label %vector.body, !llvm.loop !68

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %.sroa.12.0, %n.vec
  br i1 %cmp.n, label %.preheader.i.i.i19, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %.sroa.12.0, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.i21.preheader, label %vec.epilog.ph

.lr.ph15.i.i.i21.preheader:                       ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.i22.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec38, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i.i21

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec38 = and i64 %.sroa.12.0, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index39 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next41, %vec.epilog.vector.body ]
  %25 = getelementptr i8, ptr %16, i64 %index39
  %26 = getelementptr i8, ptr %.sroa.026.1, i64 %index39
  %wide.load40 = load <8 x i8>, ptr %26, align 1
  store <8 x i8> %wide.load40, ptr %25, align 1
  %index.next41 = add nuw i64 %index39, 8
  %27 = icmp eq i64 %index.next41, %n.vec38
  br i1 %27, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !69

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n42 = icmp eq i64 %.sroa.12.0, %n.vec38
  br i1 %cmp.n42, label %.preheader.i.i.i19, label %.lr.ph15.i.i.i21.preheader

.preheader.i.i.i19:                               ; preds = %.lr.ph15.i.i.i21, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i.i17
  tail call void @free(ptr nonnull %.sroa.026.1)
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i

.lr.ph15.i.i.i21:                                 ; preds = %.lr.ph15.i.i.i21.preheader, %.lr.ph15.i.i.i21
  %.114.i.i.i22 = phi i64 [ %31, %.lr.ph15.i.i.i21 ], [ %.114.i.i.i22.ph, %.lr.ph15.i.i.i21.preheader ]
  %28 = getelementptr i8, ptr %16, i64 %.114.i.i.i22
  %29 = getelementptr i8, ptr %.sroa.026.1, i64 %.114.i.i.i22
  %30 = load i8, ptr %29, align 1
  store i8 %30, ptr %28, align 1
  %31 = add nuw nsw i64 %.114.i.i.i22, 1
  %32 = icmp ult i64 %31, %.sroa.12.0
  br i1 %32, label %.lr.ph15.i.i.i21, label %.preheader.i.i.i19, !llvm.loop !70

rl_m_append__VectorTint8_tT_int8_t.exit.i:        ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, %.preheader.i.i.i19
  %.sroa.22.2 = phi i64 [ %15, %.preheader.i.i.i19 ], [ %.sroa.22.1, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i ]
  %.sroa.026.2 = phi ptr [ %16, %.preheader.i.i.i19 ], [ %.sroa.026.1, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i ]
  %33 = getelementptr i8, ptr %.sroa.026.2, i64 %.sroa.12.0
  %34 = load i8, ptr %11, align 1
  store i8 %34, ptr %33, align 1
  %35 = load i64, ptr %4, align 8
  %36 = icmp slt i64 %12, %35
  br i1 %36, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, label %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit

rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit:  ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i, %rl_m_init__String.exit
  %.sroa.22.3 = phi i64 [ 4, %rl_m_init__String.exit ], [ %.sroa.22.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ]
  %.sroa.12.1 = phi i64 [ 0, %rl_m_init__String.exit ], [ %12, %rl_m_append__VectorTint8_tT_int8_t.exit.i ]
  %.sroa.026.3 = phi ptr [ %7, %rl_m_init__String.exit ], [ %.sroa.026.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ]
  %37 = load i64, ptr %5, align 8
  %.not3.i.i = icmp eq i64 %37, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %38

38:                                               ; preds = %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit
  %39 = load ptr, ptr %3, align 8
  tail call void @free(ptr %39)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit, %38
  store ptr %.sroa.026.3, ptr %0, align 1
  %.sroa.0.sroa.2.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 8
  store i64 %.sroa.12.1, ptr %.sroa.0.sroa.2.0..sroa_idx, align 1
  %.sroa.0.sroa.3.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 16
  store i64 %.sroa.22.3, ptr %.sroa.0.sroa.3.0..sroa_idx, align 1
  ret void
}

; Function Attrs: nounwind
define void @rl_m_append_quoted__String_String(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %4, 0
  br i1 %5, label %rl_m_pop__VectorTint8_tT_r_int8_t.exit, label %6

6:                                                ; preds = %2
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
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
  br i1 %20, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %16
  tail call void @llvm.memset.p0.i64(ptr align 1 %18, i8 0, i64 %17, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %16
  %21 = icmp sgt i64 %11, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %21, label %iter.check, label %.preheader.i.i

iter.check:                                       ; preds = %.preheader12.i.i
  %.pre.i.i52 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %11, 8
  %22 = sub i64 %19, %.pre.i.i52
  %diff.check = icmp ult i64 %22, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check53 = icmp ult i64 %11, 32
  br i1 %min.iters.check53, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %11, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %23 = getelementptr i8, ptr %18, i64 %index
  %24 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %25 = getelementptr i8, ptr %24, i64 16
  %wide.load = load <16 x i8>, ptr %24, align 1
  %wide.load54 = load <16 x i8>, ptr %25, align 1
  %26 = getelementptr i8, ptr %23, i64 16
  store <16 x i8> %wide.load, ptr %23, align 1
  store <16 x i8> %wide.load54, ptr %26, align 1
  %index.next = add nuw i64 %index, 32
  %27 = icmp eq i64 %index.next, %n.vec
  br i1 %27, label %middle.block, label %vector.body, !llvm.loop !71

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %11, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %11, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.preheader, label %vec.epilog.ph

.lr.ph15.i.i.preheader:                           ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec56, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec56 = and i64 %11, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index57 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next59, %vec.epilog.vector.body ]
  %28 = getelementptr i8, ptr %18, i64 %index57
  %29 = getelementptr i8, ptr %.pre.i.i, i64 %index57
  %wide.load58 = load <8 x i8>, ptr %29, align 1
  store <8 x i8> %wide.load58, ptr %28, align 1
  %index.next59 = add nuw i64 %index57, 8
  %30 = icmp eq i64 %index.next59, %n.vec56
  br i1 %30, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !72

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n60 = icmp eq i64 %11, %n.vec56
  br i1 %cmp.n60, label %.preheader.i.i, label %.lr.ph15.i.i.preheader

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %17, ptr %13, align 8
  store ptr %18, ptr %0, align 8
  %.pre.i = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %34, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader ]
  %31 = getelementptr i8, ptr %18, i64 %.114.i.i
  %32 = getelementptr i8, ptr %.pre.i.i, i64 %.114.i.i
  %33 = load i8, ptr %32, align 1
  store i8 %33, ptr %31, align 1
  %34 = add nuw nsw i64 %.114.i.i, 1
  %35 = icmp slt i64 %34, %11
  br i1 %35, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !73

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %36 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %18, %.preheader.i.i ]
  %37 = phi i64 [ %11, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %38 = getelementptr i8, ptr %36, i64 %37
  store i8 34, ptr %38, align 1
  %39 = load i64, ptr %3, align 8
  %40 = add i64 %39, 1
  store i64 %40, ptr %3, align 8
  %41 = getelementptr i8, ptr %1, i64 8
  %42 = load i64, ptr %41, align 8
  %43 = add i64 %42, -1
  %44 = icmp sgt i64 %43, 0
  br i1 %44, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit, %rl_m_append__VectorTint8_tT_int8_t.exit21
  %45 = phi i64 [ %119, %rl_m_append__VectorTint8_tT_int8_t.exit21 ], [ %40, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %46 = phi i64 [ %121, %rl_m_append__VectorTint8_tT_int8_t.exit21 ], [ %42, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %storemerge51 = phi i64 [ %120, %rl_m_append__VectorTint8_tT_int8_t.exit21 ], [ 0, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %47 = icmp slt i64 %storemerge51, %46
  br i1 %47, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %48

48:                                               ; preds = %.lr.ph.preheader
  %49 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %.lr.ph.preheader
  %50 = load ptr, ptr %1, align 8
  %51 = getelementptr i8, ptr %50, i64 %storemerge51
  %52 = load i8, ptr %51, align 1
  %53 = icmp eq i8 %52, 34
  br i1 %53, label %54, label %83

54:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %55 = add i64 %45, 1
  %56 = load i64, ptr %13, align 8
  %57 = icmp sgt i64 %56, %55
  br i1 %57, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8, label %58

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8: ; preds = %54
  %.pre2.i9 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit10

58:                                               ; preds = %54
  %59 = shl i64 %55, 1
  %60 = tail call ptr @malloc(i64 %59)
  %61 = ptrtoint ptr %60 to i64
  %62 = icmp sgt i64 %59, 0
  br i1 %62, label %.lr.ph.preheader.i.i7, label %.preheader12.i.i1

.lr.ph.preheader.i.i7:                            ; preds = %58
  tail call void @llvm.memset.p0.i64(ptr align 1 %60, i8 0, i64 %59, i1 false)
  br label %.preheader12.i.i1

.preheader12.i.i1:                                ; preds = %.lr.ph.preheader.i.i7, %58
  %63 = icmp sgt i64 %45, 0
  %.pre.i.i2 = load ptr, ptr %0, align 8
  br i1 %63, label %iter.check100, label %.preheader.i.i3

iter.check100:                                    ; preds = %.preheader12.i.i1
  %.pre.i.i295 = ptrtoint ptr %.pre.i.i2 to i64
  %min.iters.check98 = icmp ult i64 %45, 8
  %64 = sub i64 %61, %.pre.i.i295
  %diff.check96 = icmp ult i64 %64, 32
  %or.cond193 = select i1 %min.iters.check98, i1 true, i1 %diff.check96
  br i1 %or.cond193, label %.lr.ph15.i.i5.preheader, label %vector.main.loop.iter.check102

vector.main.loop.iter.check102:                   ; preds = %iter.check100
  %min.iters.check101 = icmp ult i64 %45, 32
  br i1 %min.iters.check101, label %vec.epilog.ph114, label %vector.ph103

vector.ph103:                                     ; preds = %vector.main.loop.iter.check102
  %n.vec105 = and i64 %45, 9223372036854775776
  br label %vector.body106

vector.body106:                                   ; preds = %vector.body106, %vector.ph103
  %index107 = phi i64 [ 0, %vector.ph103 ], [ %index.next110, %vector.body106 ]
  %65 = getelementptr i8, ptr %60, i64 %index107
  %66 = getelementptr i8, ptr %.pre.i.i2, i64 %index107
  %67 = getelementptr i8, ptr %66, i64 16
  %wide.load108 = load <16 x i8>, ptr %66, align 1
  %wide.load109 = load <16 x i8>, ptr %67, align 1
  %68 = getelementptr i8, ptr %65, i64 16
  store <16 x i8> %wide.load108, ptr %65, align 1
  store <16 x i8> %wide.load109, ptr %68, align 1
  %index.next110 = add nuw i64 %index107, 32
  %69 = icmp eq i64 %index.next110, %n.vec105
  br i1 %69, label %middle.block97, label %vector.body106, !llvm.loop !74

middle.block97:                                   ; preds = %vector.body106
  %cmp.n111 = icmp eq i64 %45, %n.vec105
  br i1 %cmp.n111, label %.preheader.i.i3, label %vec.epilog.iter.check115

vec.epilog.iter.check115:                         ; preds = %middle.block97
  %n.vec.remaining116 = and i64 %45, 24
  %min.epilog.iters.check117 = icmp eq i64 %n.vec.remaining116, 0
  br i1 %min.epilog.iters.check117, label %.lr.ph15.i.i5.preheader, label %vec.epilog.ph114

.lr.ph15.i.i5.preheader:                          ; preds = %vec.epilog.middle.block112, %iter.check100, %vec.epilog.iter.check115
  %.114.i.i6.ph = phi i64 [ 0, %iter.check100 ], [ %n.vec105, %vec.epilog.iter.check115 ], [ %n.vec120, %vec.epilog.middle.block112 ]
  br label %.lr.ph15.i.i5

vec.epilog.ph114:                                 ; preds = %vector.main.loop.iter.check102, %vec.epilog.iter.check115
  %vec.epilog.resume.val118 = phi i64 [ %n.vec105, %vec.epilog.iter.check115 ], [ 0, %vector.main.loop.iter.check102 ]
  %n.vec120 = and i64 %45, 9223372036854775800
  br label %vec.epilog.vector.body122

vec.epilog.vector.body122:                        ; preds = %vec.epilog.vector.body122, %vec.epilog.ph114
  %index123 = phi i64 [ %vec.epilog.resume.val118, %vec.epilog.ph114 ], [ %index.next125, %vec.epilog.vector.body122 ]
  %70 = getelementptr i8, ptr %60, i64 %index123
  %71 = getelementptr i8, ptr %.pre.i.i2, i64 %index123
  %wide.load124 = load <8 x i8>, ptr %71, align 1
  store <8 x i8> %wide.load124, ptr %70, align 1
  %index.next125 = add nuw i64 %index123, 8
  %72 = icmp eq i64 %index.next125, %n.vec120
  br i1 %72, label %vec.epilog.middle.block112, label %vec.epilog.vector.body122, !llvm.loop !75

vec.epilog.middle.block112:                       ; preds = %vec.epilog.vector.body122
  %cmp.n126 = icmp eq i64 %45, %n.vec120
  br i1 %cmp.n126, label %.preheader.i.i3, label %.lr.ph15.i.i5.preheader

.preheader.i.i3:                                  ; preds = %.lr.ph15.i.i5, %middle.block97, %vec.epilog.middle.block112, %.preheader12.i.i1
  tail call void @free(ptr %.pre.i.i2)
  store i64 %59, ptr %13, align 8
  store ptr %60, ptr %0, align 8
  %.pre.i4 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit10

.lr.ph15.i.i5:                                    ; preds = %.lr.ph15.i.i5.preheader, %.lr.ph15.i.i5
  %.114.i.i6 = phi i64 [ %76, %.lr.ph15.i.i5 ], [ %.114.i.i6.ph, %.lr.ph15.i.i5.preheader ]
  %73 = getelementptr i8, ptr %60, i64 %.114.i.i6
  %74 = getelementptr i8, ptr %.pre.i.i2, i64 %.114.i.i6
  %75 = load i8, ptr %74, align 1
  store i8 %75, ptr %73, align 1
  %76 = add nuw nsw i64 %.114.i.i6, 1
  %77 = icmp slt i64 %76, %45
  br i1 %77, label %.lr.ph15.i.i5, label %.preheader.i.i3, !llvm.loop !76

rl_m_append__VectorTint8_tT_int8_t.exit10:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8, %.preheader.i.i3
  %78 = phi ptr [ %.pre2.i9, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8 ], [ %60, %.preheader.i.i3 ]
  %79 = phi i64 [ %45, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8 ], [ %.pre.i4, %.preheader.i.i3 ]
  %80 = getelementptr i8, ptr %78, i64 %79
  store i8 92, ptr %80, align 1
  %81 = load i64, ptr %3, align 8
  %82 = add i64 %81, 1
  store i64 %82, ptr %3, align 8
  %.pre = load i64, ptr %41, align 8
  br label %83

83:                                               ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit10, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %84 = phi i64 [ %82, %rl_m_append__VectorTint8_tT_int8_t.exit10 ], [ %45, %rl_m_get__String_int64_t_r_int8_tRef.exit ]
  %85 = phi i64 [ %.pre, %rl_m_append__VectorTint8_tT_int8_t.exit10 ], [ %46, %rl_m_get__String_int64_t_r_int8_tRef.exit ]
  %86 = icmp slt i64 %storemerge51, %85
  br i1 %86, label %rl_m_get__String_int64_t_r_int8_tRef.exit11, label %87

87:                                               ; preds = %83
  %88 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit11:      ; preds = %83
  %89 = load ptr, ptr %1, align 8
  %90 = getelementptr i8, ptr %89, i64 %storemerge51
  %91 = add i64 %84, 1
  %92 = load i64, ptr %13, align 8
  %93 = icmp sgt i64 %92, %91
  br i1 %93, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i19, label %94

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i19: ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit11
  %.pre2.i20 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit21

94:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit11
  %95 = shl i64 %91, 1
  %96 = tail call ptr @malloc(i64 %95)
  %97 = ptrtoint ptr %96 to i64
  %98 = icmp sgt i64 %95, 0
  br i1 %98, label %.lr.ph.preheader.i.i18, label %.preheader12.i.i12

.lr.ph.preheader.i.i18:                           ; preds = %94
  tail call void @llvm.memset.p0.i64(ptr align 1 %96, i8 0, i64 %95, i1 false)
  br label %.preheader12.i.i12

.preheader12.i.i12:                               ; preds = %.lr.ph.preheader.i.i18, %94
  %99 = icmp sgt i64 %84, 0
  %.pre.i.i13 = load ptr, ptr %0, align 8
  br i1 %99, label %iter.check67, label %.preheader.i.i14

iter.check67:                                     ; preds = %.preheader12.i.i12
  %.pre.i.i1362 = ptrtoint ptr %.pre.i.i13 to i64
  %min.iters.check65 = icmp ult i64 %84, 8
  %100 = sub i64 %97, %.pre.i.i1362
  %diff.check63 = icmp ult i64 %100, 32
  %or.cond194 = select i1 %min.iters.check65, i1 true, i1 %diff.check63
  br i1 %or.cond194, label %.lr.ph15.i.i16.preheader, label %vector.main.loop.iter.check69

vector.main.loop.iter.check69:                    ; preds = %iter.check67
  %min.iters.check68 = icmp ult i64 %84, 32
  br i1 %min.iters.check68, label %vec.epilog.ph81, label %vector.ph70

vector.ph70:                                      ; preds = %vector.main.loop.iter.check69
  %n.vec72 = and i64 %84, 9223372036854775776
  br label %vector.body73

vector.body73:                                    ; preds = %vector.body73, %vector.ph70
  %index74 = phi i64 [ 0, %vector.ph70 ], [ %index.next77, %vector.body73 ]
  %101 = getelementptr i8, ptr %96, i64 %index74
  %102 = getelementptr i8, ptr %.pre.i.i13, i64 %index74
  %103 = getelementptr i8, ptr %102, i64 16
  %wide.load75 = load <16 x i8>, ptr %102, align 1
  %wide.load76 = load <16 x i8>, ptr %103, align 1
  %104 = getelementptr i8, ptr %101, i64 16
  store <16 x i8> %wide.load75, ptr %101, align 1
  store <16 x i8> %wide.load76, ptr %104, align 1
  %index.next77 = add nuw i64 %index74, 32
  %105 = icmp eq i64 %index.next77, %n.vec72
  br i1 %105, label %middle.block64, label %vector.body73, !llvm.loop !77

middle.block64:                                   ; preds = %vector.body73
  %cmp.n78 = icmp eq i64 %84, %n.vec72
  br i1 %cmp.n78, label %.preheader.i.i14, label %vec.epilog.iter.check82

vec.epilog.iter.check82:                          ; preds = %middle.block64
  %n.vec.remaining83 = and i64 %84, 24
  %min.epilog.iters.check84 = icmp eq i64 %n.vec.remaining83, 0
  br i1 %min.epilog.iters.check84, label %.lr.ph15.i.i16.preheader, label %vec.epilog.ph81

.lr.ph15.i.i16.preheader:                         ; preds = %vec.epilog.middle.block79, %iter.check67, %vec.epilog.iter.check82
  %.114.i.i17.ph = phi i64 [ 0, %iter.check67 ], [ %n.vec72, %vec.epilog.iter.check82 ], [ %n.vec87, %vec.epilog.middle.block79 ]
  br label %.lr.ph15.i.i16

vec.epilog.ph81:                                  ; preds = %vector.main.loop.iter.check69, %vec.epilog.iter.check82
  %vec.epilog.resume.val85 = phi i64 [ %n.vec72, %vec.epilog.iter.check82 ], [ 0, %vector.main.loop.iter.check69 ]
  %n.vec87 = and i64 %84, 9223372036854775800
  br label %vec.epilog.vector.body89

vec.epilog.vector.body89:                         ; preds = %vec.epilog.vector.body89, %vec.epilog.ph81
  %index90 = phi i64 [ %vec.epilog.resume.val85, %vec.epilog.ph81 ], [ %index.next92, %vec.epilog.vector.body89 ]
  %106 = getelementptr i8, ptr %96, i64 %index90
  %107 = getelementptr i8, ptr %.pre.i.i13, i64 %index90
  %wide.load91 = load <8 x i8>, ptr %107, align 1
  store <8 x i8> %wide.load91, ptr %106, align 1
  %index.next92 = add nuw i64 %index90, 8
  %108 = icmp eq i64 %index.next92, %n.vec87
  br i1 %108, label %vec.epilog.middle.block79, label %vec.epilog.vector.body89, !llvm.loop !78

vec.epilog.middle.block79:                        ; preds = %vec.epilog.vector.body89
  %cmp.n93 = icmp eq i64 %84, %n.vec87
  br i1 %cmp.n93, label %.preheader.i.i14, label %.lr.ph15.i.i16.preheader

.preheader.i.i14:                                 ; preds = %.lr.ph15.i.i16, %middle.block64, %vec.epilog.middle.block79, %.preheader12.i.i12
  tail call void @free(ptr %.pre.i.i13)
  store i64 %95, ptr %13, align 8
  store ptr %96, ptr %0, align 8
  %.pre.i15 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit21

.lr.ph15.i.i16:                                   ; preds = %.lr.ph15.i.i16.preheader, %.lr.ph15.i.i16
  %.114.i.i17 = phi i64 [ %112, %.lr.ph15.i.i16 ], [ %.114.i.i17.ph, %.lr.ph15.i.i16.preheader ]
  %109 = getelementptr i8, ptr %96, i64 %.114.i.i17
  %110 = getelementptr i8, ptr %.pre.i.i13, i64 %.114.i.i17
  %111 = load i8, ptr %110, align 1
  store i8 %111, ptr %109, align 1
  %112 = add nuw nsw i64 %.114.i.i17, 1
  %113 = icmp slt i64 %112, %84
  br i1 %113, label %.lr.ph15.i.i16, label %.preheader.i.i14, !llvm.loop !79

rl_m_append__VectorTint8_tT_int8_t.exit21:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i19, %.preheader.i.i14
  %114 = phi ptr [ %.pre2.i20, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i19 ], [ %96, %.preheader.i.i14 ]
  %115 = phi i64 [ %84, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i19 ], [ %.pre.i15, %.preheader.i.i14 ]
  %116 = getelementptr i8, ptr %114, i64 %115
  %117 = load i8, ptr %90, align 1
  store i8 %117, ptr %116, align 1
  %118 = load i64, ptr %3, align 8
  %119 = add i64 %118, 1
  store i64 %119, ptr %3, align 8
  %120 = add nuw nsw i64 %storemerge51, 1
  %121 = load i64, ptr %41, align 8
  %122 = add i64 %121, -1
  %123 = icmp slt i64 %120, %122
  br i1 %123, label %.lr.ph.preheader, label %._crit_edge

._crit_edge:                                      ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit21, %rl_m_append__VectorTint8_tT_int8_t.exit
  %124 = phi i64 [ %40, %rl_m_append__VectorTint8_tT_int8_t.exit ], [ %119, %rl_m_append__VectorTint8_tT_int8_t.exit21 ]
  %125 = add i64 %124, 1
  %126 = load i64, ptr %13, align 8
  %127 = icmp sgt i64 %126, %125
  br i1 %127, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i29, label %128

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i29: ; preds = %._crit_edge
  %.pre2.i30 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit31

128:                                              ; preds = %._crit_edge
  %129 = shl i64 %125, 1
  %130 = tail call ptr @malloc(i64 %129)
  %131 = ptrtoint ptr %130 to i64
  %132 = icmp sgt i64 %129, 0
  br i1 %132, label %.lr.ph.preheader.i.i28, label %.preheader12.i.i22

.lr.ph.preheader.i.i28:                           ; preds = %128
  tail call void @llvm.memset.p0.i64(ptr align 1 %130, i8 0, i64 %129, i1 false)
  br label %.preheader12.i.i22

.preheader12.i.i22:                               ; preds = %.lr.ph.preheader.i.i28, %128
  %133 = icmp sgt i64 %124, 0
  %.pre.i.i23 = load ptr, ptr %0, align 8
  br i1 %133, label %iter.check133, label %.preheader.i.i24

iter.check133:                                    ; preds = %.preheader12.i.i22
  %.pre.i.i23128 = ptrtoint ptr %.pre.i.i23 to i64
  %min.iters.check131 = icmp ult i64 %124, 8
  %134 = sub i64 %131, %.pre.i.i23128
  %diff.check129 = icmp ult i64 %134, 32
  %or.cond195 = select i1 %min.iters.check131, i1 true, i1 %diff.check129
  br i1 %or.cond195, label %.lr.ph15.i.i26.preheader, label %vector.main.loop.iter.check135

vector.main.loop.iter.check135:                   ; preds = %iter.check133
  %min.iters.check134 = icmp ult i64 %124, 32
  br i1 %min.iters.check134, label %vec.epilog.ph147, label %vector.ph136

vector.ph136:                                     ; preds = %vector.main.loop.iter.check135
  %n.vec138 = and i64 %124, 9223372036854775776
  br label %vector.body139

vector.body139:                                   ; preds = %vector.body139, %vector.ph136
  %index140 = phi i64 [ 0, %vector.ph136 ], [ %index.next143, %vector.body139 ]
  %135 = getelementptr i8, ptr %130, i64 %index140
  %136 = getelementptr i8, ptr %.pre.i.i23, i64 %index140
  %137 = getelementptr i8, ptr %136, i64 16
  %wide.load141 = load <16 x i8>, ptr %136, align 1
  %wide.load142 = load <16 x i8>, ptr %137, align 1
  %138 = getelementptr i8, ptr %135, i64 16
  store <16 x i8> %wide.load141, ptr %135, align 1
  store <16 x i8> %wide.load142, ptr %138, align 1
  %index.next143 = add nuw i64 %index140, 32
  %139 = icmp eq i64 %index.next143, %n.vec138
  br i1 %139, label %middle.block130, label %vector.body139, !llvm.loop !80

middle.block130:                                  ; preds = %vector.body139
  %cmp.n144 = icmp eq i64 %124, %n.vec138
  br i1 %cmp.n144, label %.preheader.i.i24, label %vec.epilog.iter.check148

vec.epilog.iter.check148:                         ; preds = %middle.block130
  %n.vec.remaining149 = and i64 %124, 24
  %min.epilog.iters.check150 = icmp eq i64 %n.vec.remaining149, 0
  br i1 %min.epilog.iters.check150, label %.lr.ph15.i.i26.preheader, label %vec.epilog.ph147

.lr.ph15.i.i26.preheader:                         ; preds = %vec.epilog.middle.block145, %iter.check133, %vec.epilog.iter.check148
  %.114.i.i27.ph = phi i64 [ 0, %iter.check133 ], [ %n.vec138, %vec.epilog.iter.check148 ], [ %n.vec153, %vec.epilog.middle.block145 ]
  br label %.lr.ph15.i.i26

vec.epilog.ph147:                                 ; preds = %vector.main.loop.iter.check135, %vec.epilog.iter.check148
  %vec.epilog.resume.val151 = phi i64 [ %n.vec138, %vec.epilog.iter.check148 ], [ 0, %vector.main.loop.iter.check135 ]
  %n.vec153 = and i64 %124, 9223372036854775800
  br label %vec.epilog.vector.body155

vec.epilog.vector.body155:                        ; preds = %vec.epilog.vector.body155, %vec.epilog.ph147
  %index156 = phi i64 [ %vec.epilog.resume.val151, %vec.epilog.ph147 ], [ %index.next158, %vec.epilog.vector.body155 ]
  %140 = getelementptr i8, ptr %130, i64 %index156
  %141 = getelementptr i8, ptr %.pre.i.i23, i64 %index156
  %wide.load157 = load <8 x i8>, ptr %141, align 1
  store <8 x i8> %wide.load157, ptr %140, align 1
  %index.next158 = add nuw i64 %index156, 8
  %142 = icmp eq i64 %index.next158, %n.vec153
  br i1 %142, label %vec.epilog.middle.block145, label %vec.epilog.vector.body155, !llvm.loop !81

vec.epilog.middle.block145:                       ; preds = %vec.epilog.vector.body155
  %cmp.n159 = icmp eq i64 %124, %n.vec153
  br i1 %cmp.n159, label %.preheader.i.i24, label %.lr.ph15.i.i26.preheader

.preheader.i.i24:                                 ; preds = %.lr.ph15.i.i26, %middle.block130, %vec.epilog.middle.block145, %.preheader12.i.i22
  tail call void @free(ptr %.pre.i.i23)
  store i64 %129, ptr %13, align 8
  store ptr %130, ptr %0, align 8
  %.pre.i25 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit31

.lr.ph15.i.i26:                                   ; preds = %.lr.ph15.i.i26.preheader, %.lr.ph15.i.i26
  %.114.i.i27 = phi i64 [ %146, %.lr.ph15.i.i26 ], [ %.114.i.i27.ph, %.lr.ph15.i.i26.preheader ]
  %143 = getelementptr i8, ptr %130, i64 %.114.i.i27
  %144 = getelementptr i8, ptr %.pre.i.i23, i64 %.114.i.i27
  %145 = load i8, ptr %144, align 1
  store i8 %145, ptr %143, align 1
  %146 = add nuw nsw i64 %.114.i.i27, 1
  %147 = icmp slt i64 %146, %124
  br i1 %147, label %.lr.ph15.i.i26, label %.preheader.i.i24, !llvm.loop !82

rl_m_append__VectorTint8_tT_int8_t.exit31:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i29, %.preheader.i.i24
  %148 = phi ptr [ %.pre2.i30, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i29 ], [ %130, %.preheader.i.i24 ]
  %149 = phi i64 [ %124, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i29 ], [ %.pre.i25, %.preheader.i.i24 ]
  %150 = getelementptr i8, ptr %148, i64 %149
  store i8 34, ptr %150, align 1
  %151 = load i64, ptr %3, align 8
  %152 = add i64 %151, 1
  store i64 %152, ptr %3, align 8
  %153 = add i64 %151, 2
  %154 = load i64, ptr %13, align 8
  %155 = icmp sgt i64 %154, %153
  br i1 %155, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i39, label %156

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i39: ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit31
  %.pre2.i40 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit41

156:                                              ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit31
  %157 = shl i64 %153, 1
  %158 = tail call ptr @malloc(i64 %157)
  %159 = ptrtoint ptr %158 to i64
  %160 = icmp sgt i64 %157, 0
  br i1 %160, label %.lr.ph.preheader.i.i38, label %.preheader12.i.i32

.lr.ph.preheader.i.i38:                           ; preds = %156
  tail call void @llvm.memset.p0.i64(ptr align 1 %158, i8 0, i64 %157, i1 false)
  br label %.preheader12.i.i32

.preheader12.i.i32:                               ; preds = %.lr.ph.preheader.i.i38, %156
  %161 = icmp ult i64 %151, 9223372036854775807
  %.pre.i.i33 = load ptr, ptr %0, align 8
  br i1 %161, label %iter.check166, label %.preheader.i.i34

iter.check166:                                    ; preds = %.preheader12.i.i32
  %.pre.i.i33161 = ptrtoint ptr %.pre.i.i33 to i64
  %min.iters.check164 = icmp ult i64 %152, 8
  %162 = sub i64 %159, %.pre.i.i33161
  %diff.check162 = icmp ult i64 %162, 32
  %or.cond196 = select i1 %min.iters.check164, i1 true, i1 %diff.check162
  br i1 %or.cond196, label %.lr.ph15.i.i36.preheader, label %vector.main.loop.iter.check168

vector.main.loop.iter.check168:                   ; preds = %iter.check166
  %min.iters.check167 = icmp ult i64 %152, 32
  br i1 %min.iters.check167, label %vec.epilog.ph180, label %vector.ph169

vector.ph169:                                     ; preds = %vector.main.loop.iter.check168
  %n.vec171 = and i64 %152, -32
  br label %vector.body172

vector.body172:                                   ; preds = %vector.body172, %vector.ph169
  %index173 = phi i64 [ 0, %vector.ph169 ], [ %index.next176, %vector.body172 ]
  %163 = getelementptr i8, ptr %158, i64 %index173
  %164 = getelementptr i8, ptr %.pre.i.i33, i64 %index173
  %165 = getelementptr i8, ptr %164, i64 16
  %wide.load174 = load <16 x i8>, ptr %164, align 1
  %wide.load175 = load <16 x i8>, ptr %165, align 1
  %166 = getelementptr i8, ptr %163, i64 16
  store <16 x i8> %wide.load174, ptr %163, align 1
  store <16 x i8> %wide.load175, ptr %166, align 1
  %index.next176 = add nuw i64 %index173, 32
  %167 = icmp eq i64 %index.next176, %n.vec171
  br i1 %167, label %middle.block163, label %vector.body172, !llvm.loop !83

middle.block163:                                  ; preds = %vector.body172
  %cmp.n177 = icmp eq i64 %152, %n.vec171
  br i1 %cmp.n177, label %.preheader.i.i34, label %vec.epilog.iter.check181

vec.epilog.iter.check181:                         ; preds = %middle.block163
  %n.vec.remaining182 = and i64 %152, 24
  %min.epilog.iters.check183 = icmp eq i64 %n.vec.remaining182, 0
  br i1 %min.epilog.iters.check183, label %.lr.ph15.i.i36.preheader, label %vec.epilog.ph180

.lr.ph15.i.i36.preheader:                         ; preds = %vec.epilog.middle.block178, %iter.check166, %vec.epilog.iter.check181
  %.114.i.i37.ph = phi i64 [ 0, %iter.check166 ], [ %n.vec171, %vec.epilog.iter.check181 ], [ %n.vec186, %vec.epilog.middle.block178 ]
  br label %.lr.ph15.i.i36

vec.epilog.ph180:                                 ; preds = %vector.main.loop.iter.check168, %vec.epilog.iter.check181
  %vec.epilog.resume.val184 = phi i64 [ %n.vec171, %vec.epilog.iter.check181 ], [ 0, %vector.main.loop.iter.check168 ]
  %n.vec186 = and i64 %152, -8
  br label %vec.epilog.vector.body188

vec.epilog.vector.body188:                        ; preds = %vec.epilog.vector.body188, %vec.epilog.ph180
  %index189 = phi i64 [ %vec.epilog.resume.val184, %vec.epilog.ph180 ], [ %index.next191, %vec.epilog.vector.body188 ]
  %168 = getelementptr i8, ptr %158, i64 %index189
  %169 = getelementptr i8, ptr %.pre.i.i33, i64 %index189
  %wide.load190 = load <8 x i8>, ptr %169, align 1
  store <8 x i8> %wide.load190, ptr %168, align 1
  %index.next191 = add nuw i64 %index189, 8
  %170 = icmp eq i64 %index.next191, %n.vec186
  br i1 %170, label %vec.epilog.middle.block178, label %vec.epilog.vector.body188, !llvm.loop !84

vec.epilog.middle.block178:                       ; preds = %vec.epilog.vector.body188
  %cmp.n192 = icmp eq i64 %152, %n.vec186
  br i1 %cmp.n192, label %.preheader.i.i34, label %.lr.ph15.i.i36.preheader

.preheader.i.i34:                                 ; preds = %.lr.ph15.i.i36, %middle.block163, %vec.epilog.middle.block178, %.preheader12.i.i32
  tail call void @free(ptr %.pre.i.i33)
  store i64 %157, ptr %13, align 8
  store ptr %158, ptr %0, align 8
  %.pre.i35 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit41

.lr.ph15.i.i36:                                   ; preds = %.lr.ph15.i.i36.preheader, %.lr.ph15.i.i36
  %.114.i.i37 = phi i64 [ %174, %.lr.ph15.i.i36 ], [ %.114.i.i37.ph, %.lr.ph15.i.i36.preheader ]
  %171 = getelementptr i8, ptr %158, i64 %.114.i.i37
  %172 = getelementptr i8, ptr %.pre.i.i33, i64 %.114.i.i37
  %173 = load i8, ptr %172, align 1
  store i8 %173, ptr %171, align 1
  %174 = add nuw nsw i64 %.114.i.i37, 1
  %175 = icmp slt i64 %174, %152
  br i1 %175, label %.lr.ph15.i.i36, label %.preheader.i.i34, !llvm.loop !85

rl_m_append__VectorTint8_tT_int8_t.exit41:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i39, %.preheader.i.i34
  %176 = phi ptr [ %.pre2.i40, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i39 ], [ %158, %.preheader.i.i34 ]
  %177 = phi i64 [ %152, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i39 ], [ %.pre.i35, %.preheader.i.i34 ]
  %178 = getelementptr i8, ptr %176, i64 %177
  store i8 0, ptr %178, align 1
  %179 = load i64, ptr %3, align 8
  %180 = add i64 %179, 1
  store i64 %180, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_append__String_String(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %4, 0
  br i1 %5, label %rl_m_pop__VectorTint8_tT_r_int8_t.exit, label %6

6:                                                ; preds = %2
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
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
  %17 = phi i64 [ %12, %.lr.ph ], [ %54, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %storemerge15 = phi i64 [ 0, %.lr.ph ], [ %53, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %18 = icmp slt i64 %storemerge15, %17
  br i1 %18, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %19

19:                                               ; preds = %16
  %20 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  br i1 %31, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %27
  tail call void @llvm.memset.p0.i64(ptr align 1 %29, i8 0, i64 %28, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %27
  %32 = icmp sgt i64 %23, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %32, label %iter.check, label %.preheader.i.i

iter.check:                                       ; preds = %.preheader12.i.i
  %.pre.i.i16 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %23, 8
  %33 = sub i64 %30, %.pre.i.i16
  %diff.check = icmp ult i64 %33, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check17 = icmp ult i64 %23, 32
  br i1 %min.iters.check17, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %23, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %34 = getelementptr i8, ptr %29, i64 %index
  %35 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %36 = getelementptr i8, ptr %35, i64 16
  %wide.load = load <16 x i8>, ptr %35, align 1
  %wide.load18 = load <16 x i8>, ptr %36, align 1
  %37 = getelementptr i8, ptr %34, i64 16
  store <16 x i8> %wide.load, ptr %34, align 1
  store <16 x i8> %wide.load18, ptr %37, align 1
  %index.next = add nuw i64 %index, 32
  %38 = icmp eq i64 %index.next, %n.vec
  br i1 %38, label %middle.block, label %vector.body, !llvm.loop !86

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %23, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %23, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.preheader, label %vec.epilog.ph

.lr.ph15.i.i.preheader:                           ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec20, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec20 = and i64 %23, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index21 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next23, %vec.epilog.vector.body ]
  %39 = getelementptr i8, ptr %29, i64 %index21
  %40 = getelementptr i8, ptr %.pre.i.i, i64 %index21
  %wide.load22 = load <8 x i8>, ptr %40, align 1
  store <8 x i8> %wide.load22, ptr %39, align 1
  %index.next23 = add nuw i64 %index21, 8
  %41 = icmp eq i64 %index.next23, %n.vec20
  br i1 %41, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !87

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n24 = icmp eq i64 %23, %n.vec20
  br i1 %cmp.n24, label %.preheader.i.i, label %.lr.ph15.i.i.preheader

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %28, ptr %15, align 8
  store ptr %29, ptr %0, align 8
  %.pre.i = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %45, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader ]
  %42 = getelementptr i8, ptr %29, i64 %.114.i.i
  %43 = getelementptr i8, ptr %.pre.i.i, i64 %.114.i.i
  %44 = load i8, ptr %43, align 1
  store i8 %44, ptr %42, align 1
  %45 = add nuw nsw i64 %.114.i.i, 1
  %46 = icmp slt i64 %45, %23
  br i1 %46, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !88

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %47 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %29, %.preheader.i.i ]
  %48 = phi i64 [ %23, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %49 = getelementptr i8, ptr %47, i64 %48
  %50 = load i8, ptr %22, align 1
  store i8 %50, ptr %49, align 1
  %51 = load i64, ptr %3, align 8
  %52 = add i64 %51, 1
  store i64 %52, ptr %3, align 8
  %53 = add nuw nsw i64 %storemerge15, 1
  %54 = load i64, ptr %11, align 8
  %55 = add i64 %54, -1
  %56 = icmp slt i64 %53, %55
  br i1 %56, label %16, label %._crit_edge

._crit_edge:                                      ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit, %rl_m_pop__VectorTint8_tT_r_int8_t.exit.._crit_edge_crit_edge
  %57 = phi i64 [ %.pre, %rl_m_pop__VectorTint8_tT_r_int8_t.exit.._crit_edge_crit_edge ], [ %52, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %58 = add i64 %57, 1
  %59 = getelementptr i8, ptr %0, i64 16
  %60 = load i64, ptr %59, align 8
  %61 = icmp sgt i64 %60, %58
  br i1 %61, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8, label %62

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8: ; preds = %._crit_edge
  %.pre2.i9 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit10

62:                                               ; preds = %._crit_edge
  %63 = shl i64 %58, 1
  %64 = tail call ptr @malloc(i64 %63)
  %65 = ptrtoint ptr %64 to i64
  %66 = icmp sgt i64 %63, 0
  br i1 %66, label %.lr.ph.preheader.i.i7, label %.preheader12.i.i1

.lr.ph.preheader.i.i7:                            ; preds = %62
  tail call void @llvm.memset.p0.i64(ptr align 1 %64, i8 0, i64 %63, i1 false)
  br label %.preheader12.i.i1

.preheader12.i.i1:                                ; preds = %.lr.ph.preheader.i.i7, %62
  %67 = icmp sgt i64 %57, 0
  %.pre.i.i2 = load ptr, ptr %0, align 8
  br i1 %67, label %iter.check31, label %.preheader.i.i3

iter.check31:                                     ; preds = %.preheader12.i.i1
  %.pre.i.i226 = ptrtoint ptr %.pre.i.i2 to i64
  %min.iters.check29 = icmp ult i64 %57, 8
  %68 = sub i64 %65, %.pre.i.i226
  %diff.check27 = icmp ult i64 %68, 32
  %or.cond58 = select i1 %min.iters.check29, i1 true, i1 %diff.check27
  br i1 %or.cond58, label %.lr.ph15.i.i5.preheader, label %vector.main.loop.iter.check33

vector.main.loop.iter.check33:                    ; preds = %iter.check31
  %min.iters.check32 = icmp ult i64 %57, 32
  br i1 %min.iters.check32, label %vec.epilog.ph45, label %vector.ph34

vector.ph34:                                      ; preds = %vector.main.loop.iter.check33
  %n.vec36 = and i64 %57, 9223372036854775776
  br label %vector.body37

vector.body37:                                    ; preds = %vector.body37, %vector.ph34
  %index38 = phi i64 [ 0, %vector.ph34 ], [ %index.next41, %vector.body37 ]
  %69 = getelementptr i8, ptr %64, i64 %index38
  %70 = getelementptr i8, ptr %.pre.i.i2, i64 %index38
  %71 = getelementptr i8, ptr %70, i64 16
  %wide.load39 = load <16 x i8>, ptr %70, align 1
  %wide.load40 = load <16 x i8>, ptr %71, align 1
  %72 = getelementptr i8, ptr %69, i64 16
  store <16 x i8> %wide.load39, ptr %69, align 1
  store <16 x i8> %wide.load40, ptr %72, align 1
  %index.next41 = add nuw i64 %index38, 32
  %73 = icmp eq i64 %index.next41, %n.vec36
  br i1 %73, label %middle.block28, label %vector.body37, !llvm.loop !89

middle.block28:                                   ; preds = %vector.body37
  %cmp.n42 = icmp eq i64 %57, %n.vec36
  br i1 %cmp.n42, label %.preheader.i.i3, label %vec.epilog.iter.check46

vec.epilog.iter.check46:                          ; preds = %middle.block28
  %n.vec.remaining47 = and i64 %57, 24
  %min.epilog.iters.check48 = icmp eq i64 %n.vec.remaining47, 0
  br i1 %min.epilog.iters.check48, label %.lr.ph15.i.i5.preheader, label %vec.epilog.ph45

.lr.ph15.i.i5.preheader:                          ; preds = %vec.epilog.middle.block43, %iter.check31, %vec.epilog.iter.check46
  %.114.i.i6.ph = phi i64 [ 0, %iter.check31 ], [ %n.vec36, %vec.epilog.iter.check46 ], [ %n.vec51, %vec.epilog.middle.block43 ]
  br label %.lr.ph15.i.i5

vec.epilog.ph45:                                  ; preds = %vector.main.loop.iter.check33, %vec.epilog.iter.check46
  %vec.epilog.resume.val49 = phi i64 [ %n.vec36, %vec.epilog.iter.check46 ], [ 0, %vector.main.loop.iter.check33 ]
  %n.vec51 = and i64 %57, 9223372036854775800
  br label %vec.epilog.vector.body53

vec.epilog.vector.body53:                         ; preds = %vec.epilog.vector.body53, %vec.epilog.ph45
  %index54 = phi i64 [ %vec.epilog.resume.val49, %vec.epilog.ph45 ], [ %index.next56, %vec.epilog.vector.body53 ]
  %74 = getelementptr i8, ptr %64, i64 %index54
  %75 = getelementptr i8, ptr %.pre.i.i2, i64 %index54
  %wide.load55 = load <8 x i8>, ptr %75, align 1
  store <8 x i8> %wide.load55, ptr %74, align 1
  %index.next56 = add nuw i64 %index54, 8
  %76 = icmp eq i64 %index.next56, %n.vec51
  br i1 %76, label %vec.epilog.middle.block43, label %vec.epilog.vector.body53, !llvm.loop !90

vec.epilog.middle.block43:                        ; preds = %vec.epilog.vector.body53
  %cmp.n57 = icmp eq i64 %57, %n.vec51
  br i1 %cmp.n57, label %.preheader.i.i3, label %.lr.ph15.i.i5.preheader

.preheader.i.i3:                                  ; preds = %.lr.ph15.i.i5, %middle.block28, %vec.epilog.middle.block43, %.preheader12.i.i1
  tail call void @free(ptr %.pre.i.i2)
  store i64 %63, ptr %59, align 8
  store ptr %64, ptr %0, align 8
  %.pre.i4 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit10

.lr.ph15.i.i5:                                    ; preds = %.lr.ph15.i.i5.preheader, %.lr.ph15.i.i5
  %.114.i.i6 = phi i64 [ %80, %.lr.ph15.i.i5 ], [ %.114.i.i6.ph, %.lr.ph15.i.i5.preheader ]
  %77 = getelementptr i8, ptr %64, i64 %.114.i.i6
  %78 = getelementptr i8, ptr %.pre.i.i2, i64 %.114.i.i6
  %79 = load i8, ptr %78, align 1
  store i8 %79, ptr %77, align 1
  %80 = add nuw nsw i64 %.114.i.i6, 1
  %81 = icmp slt i64 %80, %57
  br i1 %81, label %.lr.ph15.i.i5, label %.preheader.i.i3, !llvm.loop !91

rl_m_append__VectorTint8_tT_int8_t.exit10:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8, %.preheader.i.i3
  %82 = phi ptr [ %.pre2.i9, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8 ], [ %64, %.preheader.i.i3 ]
  %83 = phi i64 [ %57, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i8 ], [ %.pre.i4, %.preheader.i.i3 ]
  %84 = getelementptr i8, ptr %82, i64 %83
  store i8 0, ptr %84, align 1
  %85 = load i64, ptr %3, align 8
  %86 = add i64 %85, 1
  store i64 %86, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_append__String_strlit(ptr nocapture %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
  %3 = getelementptr i8, ptr %0, i64 8
  %4 = load i64, ptr %3, align 8
  %5 = icmp sgt i64 %4, 0
  br i1 %5, label %rl_m_pop__VectorTint8_tT_r_int8_t.exit, label %6

6:                                                ; preds = %2
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_9)
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
  %.not13 = icmp eq i8 %12, 0
  %.pre16 = load i64, ptr %3, align 8
  br i1 %.not13, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %rl_m_pop__VectorTint8_tT_r_int8_t.exit
  %13 = getelementptr i8, ptr %0, i64 16
  br label %14

14:                                               ; preds = %.lr.ph, %rl_m_append__VectorTint8_tT_int8_t.exit
  %15 = phi i8 [ %12, %.lr.ph ], [ %50, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %16 = phi i64 [ %.pre16, %.lr.ph ], [ %46, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %17 = phi ptr [ %11, %.lr.ph ], [ %49, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %.014 = phi i64 [ 0, %.lr.ph ], [ %47, %rl_m_append__VectorTint8_tT_int8_t.exit ]
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
  br i1 %25, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %21
  tail call void @llvm.memset.p0.i64(ptr align 1 %23, i8 0, i64 %22, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %21
  %26 = icmp sgt i64 %16, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %26, label %iter.check, label %.preheader.i.i

iter.check:                                       ; preds = %.preheader12.i.i
  %.pre.i.i17 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %16, 8
  %27 = sub i64 %24, %.pre.i.i17
  %diff.check = icmp ult i64 %27, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check18 = icmp ult i64 %16, 32
  br i1 %min.iters.check18, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %16, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %28 = getelementptr i8, ptr %23, i64 %index
  %29 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %30 = getelementptr i8, ptr %29, i64 16
  %wide.load = load <16 x i8>, ptr %29, align 1
  %wide.load19 = load <16 x i8>, ptr %30, align 1
  %31 = getelementptr i8, ptr %28, i64 16
  store <16 x i8> %wide.load, ptr %28, align 1
  store <16 x i8> %wide.load19, ptr %31, align 1
  %index.next = add nuw i64 %index, 32
  %32 = icmp eq i64 %index.next, %n.vec
  br i1 %32, label %middle.block, label %vector.body, !llvm.loop !92

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %16, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %16, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.preheader, label %vec.epilog.ph

.lr.ph15.i.i.preheader:                           ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec21, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec21 = and i64 %16, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index22 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next24, %vec.epilog.vector.body ]
  %33 = getelementptr i8, ptr %23, i64 %index22
  %34 = getelementptr i8, ptr %.pre.i.i, i64 %index22
  %wide.load23 = load <8 x i8>, ptr %34, align 1
  store <8 x i8> %wide.load23, ptr %33, align 1
  %index.next24 = add nuw i64 %index22, 8
  %35 = icmp eq i64 %index.next24, %n.vec21
  br i1 %35, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !93

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n25 = icmp eq i64 %16, %n.vec21
  br i1 %cmp.n25, label %.preheader.i.i, label %.lr.ph15.i.i.preheader

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %22, ptr %13, align 8
  store ptr %23, ptr %0, align 8
  %.pre.i = load i64, ptr %3, align 8
  %.pre15 = load i8, ptr %17, align 1
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %39, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader ]
  %36 = getelementptr i8, ptr %23, i64 %.114.i.i
  %37 = getelementptr i8, ptr %.pre.i.i, i64 %.114.i.i
  %38 = load i8, ptr %37, align 1
  store i8 %38, ptr %36, align 1
  %39 = add nuw nsw i64 %.114.i.i, 1
  %40 = icmp slt i64 %39, %16
  br i1 %40, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !94

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %41 = phi i8 [ %15, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre15, %.preheader.i.i ]
  %42 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %23, %.preheader.i.i ]
  %43 = phi i64 [ %16, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %44 = getelementptr i8, ptr %42, i64 %43
  store i8 %41, ptr %44, align 1
  %45 = load i64, ptr %3, align 8
  %46 = add i64 %45, 1
  store i64 %46, ptr %3, align 8
  %47 = add i64 %.014, 1
  %48 = load ptr, ptr %1, align 8
  %49 = getelementptr i8, ptr %48, i64 %47
  %50 = load i8, ptr %49, align 1
  %.not = icmp eq i8 %50, 0
  br i1 %.not, label %._crit_edge, label %14

._crit_edge:                                      ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit, %rl_m_pop__VectorTint8_tT_r_int8_t.exit
  %51 = phi i64 [ %.pre16, %rl_m_pop__VectorTint8_tT_r_int8_t.exit ], [ %46, %rl_m_append__VectorTint8_tT_int8_t.exit ]
  %52 = add i64 %51, 1
  %53 = getelementptr i8, ptr %0, i64 16
  %54 = load i64, ptr %53, align 8
  %55 = icmp sgt i64 %54, %52
  br i1 %55, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10, label %56

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10: ; preds = %._crit_edge
  %.pre2.i11 = load ptr, ptr %0, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit12

56:                                               ; preds = %._crit_edge
  %57 = shl i64 %52, 1
  %58 = tail call ptr @malloc(i64 %57)
  %59 = ptrtoint ptr %58 to i64
  %60 = icmp sgt i64 %57, 0
  br i1 %60, label %.lr.ph.preheader.i.i9, label %.preheader12.i.i3

.lr.ph.preheader.i.i9:                            ; preds = %56
  tail call void @llvm.memset.p0.i64(ptr align 1 %58, i8 0, i64 %57, i1 false)
  br label %.preheader12.i.i3

.preheader12.i.i3:                                ; preds = %.lr.ph.preheader.i.i9, %56
  %61 = icmp sgt i64 %51, 0
  %.pre.i.i4 = load ptr, ptr %0, align 8
  br i1 %61, label %iter.check32, label %.preheader.i.i5

iter.check32:                                     ; preds = %.preheader12.i.i3
  %.pre.i.i427 = ptrtoint ptr %.pre.i.i4 to i64
  %min.iters.check30 = icmp ult i64 %51, 8
  %62 = sub i64 %59, %.pre.i.i427
  %diff.check28 = icmp ult i64 %62, 32
  %or.cond59 = select i1 %min.iters.check30, i1 true, i1 %diff.check28
  br i1 %or.cond59, label %.lr.ph15.i.i7.preheader, label %vector.main.loop.iter.check34

vector.main.loop.iter.check34:                    ; preds = %iter.check32
  %min.iters.check33 = icmp ult i64 %51, 32
  br i1 %min.iters.check33, label %vec.epilog.ph46, label %vector.ph35

vector.ph35:                                      ; preds = %vector.main.loop.iter.check34
  %n.vec37 = and i64 %51, 9223372036854775776
  br label %vector.body38

vector.body38:                                    ; preds = %vector.body38, %vector.ph35
  %index39 = phi i64 [ 0, %vector.ph35 ], [ %index.next42, %vector.body38 ]
  %63 = getelementptr i8, ptr %58, i64 %index39
  %64 = getelementptr i8, ptr %.pre.i.i4, i64 %index39
  %65 = getelementptr i8, ptr %64, i64 16
  %wide.load40 = load <16 x i8>, ptr %64, align 1
  %wide.load41 = load <16 x i8>, ptr %65, align 1
  %66 = getelementptr i8, ptr %63, i64 16
  store <16 x i8> %wide.load40, ptr %63, align 1
  store <16 x i8> %wide.load41, ptr %66, align 1
  %index.next42 = add nuw i64 %index39, 32
  %67 = icmp eq i64 %index.next42, %n.vec37
  br i1 %67, label %middle.block29, label %vector.body38, !llvm.loop !95

middle.block29:                                   ; preds = %vector.body38
  %cmp.n43 = icmp eq i64 %51, %n.vec37
  br i1 %cmp.n43, label %.preheader.i.i5, label %vec.epilog.iter.check47

vec.epilog.iter.check47:                          ; preds = %middle.block29
  %n.vec.remaining48 = and i64 %51, 24
  %min.epilog.iters.check49 = icmp eq i64 %n.vec.remaining48, 0
  br i1 %min.epilog.iters.check49, label %.lr.ph15.i.i7.preheader, label %vec.epilog.ph46

.lr.ph15.i.i7.preheader:                          ; preds = %vec.epilog.middle.block44, %iter.check32, %vec.epilog.iter.check47
  %.114.i.i8.ph = phi i64 [ 0, %iter.check32 ], [ %n.vec37, %vec.epilog.iter.check47 ], [ %n.vec52, %vec.epilog.middle.block44 ]
  br label %.lr.ph15.i.i7

vec.epilog.ph46:                                  ; preds = %vector.main.loop.iter.check34, %vec.epilog.iter.check47
  %vec.epilog.resume.val50 = phi i64 [ %n.vec37, %vec.epilog.iter.check47 ], [ 0, %vector.main.loop.iter.check34 ]
  %n.vec52 = and i64 %51, 9223372036854775800
  br label %vec.epilog.vector.body54

vec.epilog.vector.body54:                         ; preds = %vec.epilog.vector.body54, %vec.epilog.ph46
  %index55 = phi i64 [ %vec.epilog.resume.val50, %vec.epilog.ph46 ], [ %index.next57, %vec.epilog.vector.body54 ]
  %68 = getelementptr i8, ptr %58, i64 %index55
  %69 = getelementptr i8, ptr %.pre.i.i4, i64 %index55
  %wide.load56 = load <8 x i8>, ptr %69, align 1
  store <8 x i8> %wide.load56, ptr %68, align 1
  %index.next57 = add nuw i64 %index55, 8
  %70 = icmp eq i64 %index.next57, %n.vec52
  br i1 %70, label %vec.epilog.middle.block44, label %vec.epilog.vector.body54, !llvm.loop !96

vec.epilog.middle.block44:                        ; preds = %vec.epilog.vector.body54
  %cmp.n58 = icmp eq i64 %51, %n.vec52
  br i1 %cmp.n58, label %.preheader.i.i5, label %.lr.ph15.i.i7.preheader

.preheader.i.i5:                                  ; preds = %.lr.ph15.i.i7, %middle.block29, %vec.epilog.middle.block44, %.preheader12.i.i3
  tail call void @free(ptr %.pre.i.i4)
  store i64 %57, ptr %53, align 8
  store ptr %58, ptr %0, align 8
  %.pre.i6 = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit12

.lr.ph15.i.i7:                                    ; preds = %.lr.ph15.i.i7.preheader, %.lr.ph15.i.i7
  %.114.i.i8 = phi i64 [ %74, %.lr.ph15.i.i7 ], [ %.114.i.i8.ph, %.lr.ph15.i.i7.preheader ]
  %71 = getelementptr i8, ptr %58, i64 %.114.i.i8
  %72 = getelementptr i8, ptr %.pre.i.i4, i64 %.114.i.i8
  %73 = load i8, ptr %72, align 1
  store i8 %73, ptr %71, align 1
  %74 = add nuw nsw i64 %.114.i.i8, 1
  %75 = icmp slt i64 %74, %51
  br i1 %75, label %.lr.ph15.i.i7, label %.preheader.i.i5, !llvm.loop !97

rl_m_append__VectorTint8_tT_int8_t.exit12:        ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10, %.preheader.i.i5
  %76 = phi ptr [ %.pre2.i11, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10 ], [ %58, %.preheader.i.i5 ]
  %77 = phi i64 [ %51, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i10 ], [ %.pre.i6, %.preheader.i.i5 ]
  %78 = getelementptr i8, ptr %76, i64 %77
  store i8 0, ptr %78, align 1
  %79 = load i64, ptr %3, align 8
  %80 = add i64 %79, 1
  store i64 %80, ptr %3, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_m_count__String_int8_t_r_int64_t(ptr nocapture writeonly %0, ptr nocapture readonly %1, ptr nocapture readonly %2) local_unnamed_addr #5 {
  %4 = getelementptr i8, ptr %1, i64 8
  %5 = load i64, ptr %4, align 8
  %6 = add i64 %5, -1
  %.not6 = icmp eq i64 %6, 0
  br i1 %.not6, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %3
  %smax = tail call i64 @llvm.smax.i64(i64 %5, i64 0)
  %7 = add i64 %5, -2
  %.not10.not = icmp ugt i64 %smax, %7
  br i1 %.not10.not, label %.lr.ph.preheader.split, label %23

.lr.ph.preheader.split:                           ; preds = %.lr.ph.preheader
  %.pre = load ptr, ptr %1, align 8
  %.pre9 = load i8, ptr %2, align 1
  %min.iters.check = icmp ult i64 %6, 4
  br i1 %min.iters.check, label %.lr.ph.preheader13, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader.split
  %n.vec = and i64 %6, -4
  %broadcast.splatinsert = insertelement <2 x i8> poison, i8 %.pre9, i64 0
  %broadcast.splat = shufflevector <2 x i8> %broadcast.splatinsert, <2 x i8> poison, <2 x i32> zeroinitializer
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi <2 x i64> [ zeroinitializer, %vector.ph ], [ %14, %vector.body ]
  %vec.phi11 = phi <2 x i64> [ zeroinitializer, %vector.ph ], [ %15, %vector.body ]
  %8 = getelementptr i8, ptr %.pre, i64 %index
  %9 = getelementptr i8, ptr %8, i64 2
  %wide.load = load <2 x i8>, ptr %8, align 1
  %wide.load12 = load <2 x i8>, ptr %9, align 1
  %10 = icmp eq <2 x i8> %wide.load, %broadcast.splat
  %11 = icmp eq <2 x i8> %wide.load12, %broadcast.splat
  %12 = zext <2 x i1> %10 to <2 x i64>
  %13 = zext <2 x i1> %11 to <2 x i64>
  %14 = add <2 x i64> %vec.phi, %12
  %15 = add <2 x i64> %vec.phi11, %13
  %index.next = add nuw i64 %index, 4
  %16 = icmp eq i64 %index.next, %n.vec
  br i1 %16, label %middle.block, label %vector.body, !llvm.loop !98

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <2 x i64> %15, %14
  %17 = tail call i64 @llvm.vector.reduce.add.v2i64(<2 x i64> %bin.rdx)
  %cmp.n = icmp eq i64 %6, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %.lr.ph.preheader13

.lr.ph.preheader13:                               ; preds = %middle.block, %.lr.ph.preheader.split
  %.08.ph = phi i64 [ 0, %.lr.ph.preheader.split ], [ %17, %middle.block ]
  %storemerge7.ph = phi i64 [ 0, %.lr.ph.preheader.split ], [ %n.vec, %middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader13, %.lr.ph
  %.08 = phi i64 [ %spec.select, %.lr.ph ], [ %.08.ph, %.lr.ph.preheader13 ]
  %storemerge7 = phi i64 [ %22, %.lr.ph ], [ %storemerge7.ph, %.lr.ph.preheader13 ]
  %18 = getelementptr i8, ptr %.pre, i64 %storemerge7
  %19 = load i8, ptr %18, align 1
  %20 = icmp eq i8 %19, %.pre9
  %21 = zext i1 %20 to i64
  %spec.select = add i64 %.08, %21
  %22 = add nuw nsw i64 %storemerge7, 1
  %.not = icmp eq i64 %22, %6
  br i1 %.not, label %._crit_edge, label %.lr.ph, !llvm.loop !99

23:                                               ; preds = %.lr.ph.preheader
  %24 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %3
  %.0.lcssa = phi i64 [ 0, %3 ], [ %17, %middle.block ], [ %spec.select, %.lr.ph ]
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
  %.not69 = icmp eq i8 %10, 0
  br i1 %.not69, label %common.ret, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %.preheader
  %11 = icmp sgt i64 %8, -1
  br label %.lr.ph

12:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %13 = add i64 %.010, 1
  %14 = getelementptr i8, ptr %9, i64 %13
  %15 = load i8, ptr %14, align 1
  %.not6 = icmp eq i8 %15, 0
  br i1 %.not6, label %common.ret, label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %12
  %16 = phi i8 [ %15, %12 ], [ %10, %.lr.ph.preheader ]
  %.010 = phi i64 [ %13, %12 ], [ 0, %.lr.ph.preheader ]
  %17 = add nuw i64 %.010, %8
  br i1 %11, label %20, label %18

18:                                               ; preds = %.lr.ph
  %19 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

20:                                               ; preds = %.lr.ph
  %21 = icmp slt i64 %17, %6
  br i1 %21, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %22

22:                                               ; preds = %20
  %23 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %20
  %24 = load ptr, ptr %1, align 8
  %25 = getelementptr i8, ptr %24, i64 %17
  %26 = load i8, ptr %25, align 1
  %.not7 = icmp eq i8 %16, %26
  br i1 %.not7, label %12, label %common.ret

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
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

8:                                                ; preds = %3
  %9 = getelementptr i8, ptr %1, i64 8
  %10 = load i64, ptr %9, align 8
  %11 = icmp slt i64 %4, %10
  br i1 %11, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit, label %12

12:                                               ; preds = %8
  %13 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
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
  br i1 %21, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %17
  tail call void @llvm.memset.p0.i64(ptr align 1 %19, i8 0, i64 %18, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %17
  %22 = icmp sgt i64 %12, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %22, label %iter.check, label %.preheader.i.i

iter.check:                                       ; preds = %.preheader12.i.i
  %.pre.i.i1 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %12, 8
  %23 = sub i64 %20, %.pre.i.i1
  %diff.check = icmp ult i64 %23, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check2 = icmp ult i64 %12, 32
  br i1 %min.iters.check2, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %12, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %24 = getelementptr i8, ptr %19, i64 %index
  %25 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %26 = getelementptr i8, ptr %25, i64 16
  %wide.load = load <16 x i8>, ptr %25, align 1
  %wide.load3 = load <16 x i8>, ptr %26, align 1
  %27 = getelementptr i8, ptr %24, i64 16
  store <16 x i8> %wide.load, ptr %24, align 1
  store <16 x i8> %wide.load3, ptr %27, align 1
  %index.next = add nuw i64 %index, 32
  %28 = icmp eq i64 %index.next, %n.vec
  br i1 %28, label %middle.block, label %vector.body, !llvm.loop !100

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %12, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %12, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.preheader, label %vec.epilog.ph

.lr.ph15.i.i.preheader:                           ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec5, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec5 = and i64 %12, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index6 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next8, %vec.epilog.vector.body ]
  %29 = getelementptr i8, ptr %19, i64 %index6
  %30 = getelementptr i8, ptr %.pre.i.i, i64 %index6
  %wide.load7 = load <8 x i8>, ptr %30, align 1
  store <8 x i8> %wide.load7, ptr %29, align 1
  %index.next8 = add nuw i64 %index6, 8
  %31 = icmp eq i64 %index.next8, %n.vec5
  br i1 %31, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !101

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n9 = icmp eq i64 %12, %n.vec5
  br i1 %cmp.n9, label %.preheader.i.i, label %.lr.ph15.i.i.preheader

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %18, ptr %14, align 8
  store ptr %19, ptr %0, align 8
  %.pre.i = load i64, ptr %3, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %35, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader ]
  %32 = getelementptr i8, ptr %19, i64 %.114.i.i
  %33 = getelementptr i8, ptr %.pre.i.i, i64 %.114.i.i
  %34 = load i8, ptr %33, align 1
  store i8 %34, ptr %32, align 1
  %35 = add nuw nsw i64 %.114.i.i, 1
  %36 = icmp slt i64 %35, %12
  br i1 %36, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !102

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %37 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %19, %.preheader.i.i ]
  %38 = phi i64 [ %12, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %39 = getelementptr i8, ptr %37, i64 %38
  store i8 0, ptr %39, align 1
  %40 = load i64, ptr %3, align 8
  %41 = add i64 %40, 1
  store i64 %41, ptr %3, align 8
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
  %.03.i = phi i64 [ %7, %.lr.ph.i ], [ 0, %1 ]
  %5 = load ptr, ptr %0, align 8
  %6 = getelementptr i8, ptr %5, i64 %.03.i
  store i8 0, ptr %6, align 1
  %7 = add nuw nsw i64 %.03.i, 1
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
  br i1 %17, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %13
  tail call void @llvm.memset.p0.i64(ptr align 1 %15, i8 0, i64 %14, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %13
  %18 = icmp sgt i64 %10, 0
  %.pre.i.i = load ptr, ptr %0, align 8
  br i1 %18, label %iter.check, label %.preheader.i.i

iter.check:                                       ; preds = %.preheader12.i.i
  %.pre.i.i1 = ptrtoint ptr %.pre.i.i to i64
  %min.iters.check = icmp ult i64 %10, 8
  %19 = sub i64 %16, %.pre.i.i1
  %diff.check = icmp ult i64 %19, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check2 = icmp ult i64 %10, 32
  br i1 %min.iters.check2, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %10, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %20 = getelementptr i8, ptr %15, i64 %index
  %21 = getelementptr i8, ptr %.pre.i.i, i64 %index
  %22 = getelementptr i8, ptr %21, i64 16
  %wide.load = load <16 x i8>, ptr %21, align 1
  %wide.load3 = load <16 x i8>, ptr %22, align 1
  %23 = getelementptr i8, ptr %20, i64 16
  store <16 x i8> %wide.load, ptr %20, align 1
  store <16 x i8> %wide.load3, ptr %23, align 1
  %index.next = add nuw i64 %index, 32
  %24 = icmp eq i64 %index.next, %n.vec
  br i1 %24, label %middle.block, label %vector.body, !llvm.loop !103

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %10, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %10, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.preheader, label %vec.epilog.ph

.lr.ph15.i.i.preheader:                           ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec5, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec5 = and i64 %10, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index6 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next8, %vec.epilog.vector.body ]
  %25 = getelementptr i8, ptr %15, i64 %index6
  %26 = getelementptr i8, ptr %.pre.i.i, i64 %index6
  %wide.load7 = load <8 x i8>, ptr %26, align 1
  store <8 x i8> %wide.load7, ptr %25, align 1
  %index.next8 = add nuw i64 %index6, 8
  %27 = icmp eq i64 %index.next8, %n.vec5
  br i1 %27, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !104

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n9 = icmp eq i64 %10, %n.vec5
  br i1 %cmp.n9, label %.preheader.i.i, label %.lr.ph15.i.i.preheader

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i
  tail call void @free(ptr %.pre.i.i)
  store i64 %14, ptr %3, align 8
  store ptr %15, ptr %0, align 8
  %.pre.i = load i64, ptr %2, align 8
  br label %rl_m_append__VectorTint8_tT_int8_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %31, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader ]
  %28 = getelementptr i8, ptr %15, i64 %.114.i.i
  %29 = getelementptr i8, ptr %.pre.i.i, i64 %.114.i.i
  %30 = load i8, ptr %29, align 1
  store i8 %30, ptr %28, align 1
  %31 = add nuw nsw i64 %.114.i.i, 1
  %32 = icmp slt i64 %31, %10
  br i1 %32, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !105

rl_m_append__VectorTint8_tT_int8_t.exit:          ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i, %.preheader.i.i
  %33 = phi ptr [ %.pre2.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %15, %.preheader.i.i ]
  %34 = phi i64 [ %10, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i ], [ %.pre.i, %.preheader.i.i ]
  %35 = getelementptr i8, ptr %33, i64 %34
  store i8 0, ptr %35, align 1
  %36 = load i64, ptr %2, align 8
  %37 = add i64 %36, 1
  store i64 %37, ptr %2, align 8
  ret void
}

; Function Attrs: nounwind
define void @rl_s__strlit_r_String(ptr nocapture writeonly %0, ptr nocapture readonly %1) local_unnamed_addr #5 {
rl_m_init__String.exit:
  %2 = alloca %String, align 8
  %3 = getelementptr inbounds i8, ptr %2, i64 8
  %4 = getelementptr inbounds i8, ptr %2, i64 16
  store i64 4, ptr %4, align 8
  %5 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %5, ptr %2, align 8
  store i32 0, ptr %5, align 1
  store i64 1, ptr %3, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %2, ptr %1)
  %6 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store i32 0, ptr %6, align 1
  %7 = load i64, ptr %3, align 8
  %8 = icmp sgt i64 %7, 0
  br i1 %8, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, label %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit

rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i: ; preds = %rl_m_init__String.exit, %rl_m_append__VectorTint8_tT_int8_t.exit.i
  %.sroa.22.1 = phi i64 [ %.sroa.22.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ], [ 4, %rl_m_init__String.exit ]
  %.sroa.12.0 = phi i64 [ %11, %rl_m_append__VectorTint8_tT_int8_t.exit.i ], [ 0, %rl_m_init__String.exit ]
  %.sroa.026.1 = phi ptr [ %.sroa.026.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ], [ %6, %rl_m_init__String.exit ]
  %.sroa.026.134 = ptrtoint ptr %.sroa.026.1 to i64
  %9 = load ptr, ptr %2, align 8
  %10 = getelementptr i8, ptr %9, i64 %.sroa.12.0
  %11 = add nuw nsw i64 %.sroa.12.0, 1
  %12 = icmp sgt i64 %.sroa.22.1, %11
  br i1 %12, label %rl_m_append__VectorTint8_tT_int8_t.exit.i, label %13

13:                                               ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i
  %14 = shl nuw i64 %11, 1
  %15 = tail call ptr @malloc(i64 %14)
  %16 = ptrtoint ptr %15 to i64
  %17 = icmp sgt i64 %14, 0
  br i1 %17, label %.lr.ph.preheader.i.i.i23, label %.preheader12.i.i.i17

.lr.ph.preheader.i.i.i23:                         ; preds = %13
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(1) %15, i8 0, i64 %14, i1 false)
  br label %.preheader12.i.i.i17

.preheader12.i.i.i17:                             ; preds = %.lr.ph.preheader.i.i.i23, %13
  %.not = icmp eq i64 %.sroa.12.0, 0
  br i1 %.not, label %.preheader.i.i.i19, label %iter.check

iter.check:                                       ; preds = %.preheader12.i.i.i17
  %min.iters.check = icmp ult i64 %.sroa.12.0, 8
  %18 = sub i64 %16, %.sroa.026.134
  %diff.check = icmp ult i64 %18, 32
  %or.cond = or i1 %min.iters.check, %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.i21.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check35 = icmp ult i64 %.sroa.12.0, 32
  br i1 %min.iters.check35, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %.sroa.12.0, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %19 = getelementptr i8, ptr %15, i64 %index
  %20 = getelementptr i8, ptr %.sroa.026.1, i64 %index
  %21 = getelementptr i8, ptr %20, i64 16
  %wide.load = load <16 x i8>, ptr %20, align 1
  %wide.load36 = load <16 x i8>, ptr %21, align 1
  %22 = getelementptr i8, ptr %19, i64 16
  store <16 x i8> %wide.load, ptr %19, align 1
  store <16 x i8> %wide.load36, ptr %22, align 1
  %index.next = add nuw i64 %index, 32
  %23 = icmp eq i64 %index.next, %n.vec
  br i1 %23, label %middle.block, label %vector.body, !llvm.loop !106

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %.sroa.12.0, %n.vec
  br i1 %cmp.n, label %.preheader.i.i.i19, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %.sroa.12.0, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.i21.preheader, label %vec.epilog.ph

.lr.ph15.i.i.i21.preheader:                       ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.i22.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec38, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i.i21

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec38 = and i64 %.sroa.12.0, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index39 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next41, %vec.epilog.vector.body ]
  %24 = getelementptr i8, ptr %15, i64 %index39
  %25 = getelementptr i8, ptr %.sroa.026.1, i64 %index39
  %wide.load40 = load <8 x i8>, ptr %25, align 1
  store <8 x i8> %wide.load40, ptr %24, align 1
  %index.next41 = add nuw i64 %index39, 8
  %26 = icmp eq i64 %index.next41, %n.vec38
  br i1 %26, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !107

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n42 = icmp eq i64 %.sroa.12.0, %n.vec38
  br i1 %cmp.n42, label %.preheader.i.i.i19, label %.lr.ph15.i.i.i21.preheader

.preheader.i.i.i19:                               ; preds = %.lr.ph15.i.i.i21, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i.i17
  tail call void @free(ptr nonnull %.sroa.026.1)
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i

.lr.ph15.i.i.i21:                                 ; preds = %.lr.ph15.i.i.i21.preheader, %.lr.ph15.i.i.i21
  %.114.i.i.i22 = phi i64 [ %30, %.lr.ph15.i.i.i21 ], [ %.114.i.i.i22.ph, %.lr.ph15.i.i.i21.preheader ]
  %27 = getelementptr i8, ptr %15, i64 %.114.i.i.i22
  %28 = getelementptr i8, ptr %.sroa.026.1, i64 %.114.i.i.i22
  %29 = load i8, ptr %28, align 1
  store i8 %29, ptr %27, align 1
  %30 = add nuw nsw i64 %.114.i.i.i22, 1
  %31 = icmp ult i64 %30, %.sroa.12.0
  br i1 %31, label %.lr.ph15.i.i.i21, label %.preheader.i.i.i19, !llvm.loop !108

rl_m_append__VectorTint8_tT_int8_t.exit.i:        ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, %.preheader.i.i.i19
  %.sroa.22.2 = phi i64 [ %14, %.preheader.i.i.i19 ], [ %.sroa.22.1, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i ]
  %.sroa.026.2 = phi ptr [ %15, %.preheader.i.i.i19 ], [ %.sroa.026.1, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i ]
  %32 = getelementptr i8, ptr %.sroa.026.2, i64 %.sroa.12.0
  %33 = load i8, ptr %10, align 1
  store i8 %33, ptr %32, align 1
  %34 = load i64, ptr %3, align 8
  %35 = icmp slt i64 %11, %34
  br i1 %35, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, label %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit

rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit:  ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i, %rl_m_init__String.exit
  %.sroa.22.3 = phi i64 [ 4, %rl_m_init__String.exit ], [ %.sroa.22.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ]
  %.sroa.12.1 = phi i64 [ 0, %rl_m_init__String.exit ], [ %11, %rl_m_append__VectorTint8_tT_int8_t.exit.i ]
  %.sroa.026.3 = phi ptr [ %6, %rl_m_init__String.exit ], [ %.sroa.026.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ]
  %36 = load i64, ptr %4, align 8
  %.not3.i.i = icmp eq i64 %36, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %37

37:                                               ; preds = %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit
  %38 = load ptr, ptr %2, align 8
  tail call void @free(ptr %38)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit, %37
  store ptr %.sroa.026.3, ptr %0, align 1
  %.sroa.0.sroa.2.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 8
  store i64 %.sroa.12.1, ptr %.sroa.0.sroa.2.0..sroa_idx, align 1
  %.sroa.0.sroa.3.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 16
  store i64 %.sroa.22.3, ptr %.sroa.0.sroa.3.0..sroa_idx, align 1
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
  store ptr @str_14, ptr %4, align 8
  br label %8

7:                                                ; preds = %2
  store ptr @str_15, ptr %3, align 8
  br label %8

8:                                                ; preds = %7, %6
  %.sink = phi ptr [ %3, %7 ], [ %4, %6 ]
  call void @rl_m_append__String_strlit(ptr %1, ptr nonnull %.sink)
  ret void
}

define void @rl_to_string__int64_t_r_String(ptr nocapture writeonly %0, ptr %1) local_unnamed_addr {
vector.ph:
  %2 = alloca %String, align 8
  %3 = getelementptr inbounds i8, ptr %2, i64 8
  %4 = getelementptr inbounds i8, ptr %2, i64 16
  store i64 4, ptr %4, align 8
  %5 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %5, ptr %2, align 8
  store <4 x i8> zeroinitializer, ptr %5, align 1
  store i8 0, ptr %5, align 1
  store i64 1, ptr %3, align 8
  call void @rl_append_to_string__int64_t_String(ptr %1, ptr nonnull %2)
  %6 = call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store i32 0, ptr %6, align 1
  %7 = load i64, ptr %3, align 8
  %8 = icmp sgt i64 %7, 0
  br i1 %8, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, label %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit

rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i: ; preds = %vector.ph, %rl_m_append__VectorTint8_tT_int8_t.exit.i
  %.sroa.22.1 = phi i64 [ %.sroa.22.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ], [ 4, %vector.ph ]
  %.sroa.12.0 = phi i64 [ %11, %rl_m_append__VectorTint8_tT_int8_t.exit.i ], [ 0, %vector.ph ]
  %.sroa.026.1 = phi ptr [ %.sroa.026.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ], [ %6, %vector.ph ]
  %.sroa.026.136 = ptrtoint ptr %.sroa.026.1 to i64
  %9 = load ptr, ptr %2, align 8
  %10 = getelementptr i8, ptr %9, i64 %.sroa.12.0
  %11 = add nuw nsw i64 %.sroa.12.0, 1
  %12 = icmp sgt i64 %.sroa.22.1, %11
  br i1 %12, label %rl_m_append__VectorTint8_tT_int8_t.exit.i, label %13

13:                                               ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i
  %14 = shl nuw i64 %11, 1
  %15 = call ptr @malloc(i64 %14)
  %16 = ptrtoint ptr %15 to i64
  %17 = icmp sgt i64 %14, 0
  br i1 %17, label %.lr.ph.preheader.i.i.i23, label %.preheader12.i.i.i17

.lr.ph.preheader.i.i.i23:                         ; preds = %13
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(1) %15, i8 0, i64 %14, i1 false)
  br label %.preheader12.i.i.i17

.preheader12.i.i.i17:                             ; preds = %.lr.ph.preheader.i.i.i23, %13
  %.not = icmp eq i64 %.sroa.12.0, 0
  br i1 %.not, label %.preheader.i.i.i19, label %iter.check

iter.check:                                       ; preds = %.preheader12.i.i.i17
  %min.iters.check = icmp ult i64 %.sroa.12.0, 8
  %18 = sub i64 %16, %.sroa.026.136
  %diff.check = icmp ult i64 %18, 32
  %or.cond = or i1 %min.iters.check, %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.i21.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check40 = icmp ult i64 %.sroa.12.0, 32
  br i1 %min.iters.check40, label %vec.epilog.ph, label %vector.ph41

vector.ph41:                                      ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %.sroa.12.0, 9223372036854775776
  br label %vector.body42

vector.body42:                                    ; preds = %vector.body42, %vector.ph41
  %index43 = phi i64 [ 0, %vector.ph41 ], [ %index.next45, %vector.body42 ]
  %19 = getelementptr i8, ptr %15, i64 %index43
  %20 = getelementptr i8, ptr %.sroa.026.1, i64 %index43
  %21 = getelementptr i8, ptr %20, i64 16
  %wide.load = load <16 x i8>, ptr %20, align 1
  %wide.load44 = load <16 x i8>, ptr %21, align 1
  %22 = getelementptr i8, ptr %19, i64 16
  store <16 x i8> %wide.load, ptr %19, align 1
  store <16 x i8> %wide.load44, ptr %22, align 1
  %index.next45 = add nuw i64 %index43, 32
  %23 = icmp eq i64 %index.next45, %n.vec
  br i1 %23, label %middle.block37, label %vector.body42, !llvm.loop !109

middle.block37:                                   ; preds = %vector.body42
  %cmp.n = icmp eq i64 %.sroa.12.0, %n.vec
  br i1 %cmp.n, label %.preheader.i.i.i19, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block37
  %n.vec.remaining = and i64 %.sroa.12.0, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.i21.preheader, label %vec.epilog.ph

.lr.ph15.i.i.i21.preheader:                       ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.i22.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec47, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i.i21

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec47 = and i64 %.sroa.12.0, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index49 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next51, %vec.epilog.vector.body ]
  %24 = getelementptr i8, ptr %15, i64 %index49
  %25 = getelementptr i8, ptr %.sroa.026.1, i64 %index49
  %wide.load50 = load <8 x i8>, ptr %25, align 1
  store <8 x i8> %wide.load50, ptr %24, align 1
  %index.next51 = add nuw i64 %index49, 8
  %26 = icmp eq i64 %index.next51, %n.vec47
  br i1 %26, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !110

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n52 = icmp eq i64 %.sroa.12.0, %n.vec47
  br i1 %cmp.n52, label %.preheader.i.i.i19, label %.lr.ph15.i.i.i21.preheader

.preheader.i.i.i19:                               ; preds = %.lr.ph15.i.i.i21, %middle.block37, %vec.epilog.middle.block, %.preheader12.i.i.i17
  call void @free(ptr nonnull %.sroa.026.1)
  br label %rl_m_append__VectorTint8_tT_int8_t.exit.i

.lr.ph15.i.i.i21:                                 ; preds = %.lr.ph15.i.i.i21.preheader, %.lr.ph15.i.i.i21
  %.114.i.i.i22 = phi i64 [ %30, %.lr.ph15.i.i.i21 ], [ %.114.i.i.i22.ph, %.lr.ph15.i.i.i21.preheader ]
  %27 = getelementptr i8, ptr %15, i64 %.114.i.i.i22
  %28 = getelementptr i8, ptr %.sroa.026.1, i64 %.114.i.i.i22
  %29 = load i8, ptr %28, align 1
  store i8 %29, ptr %27, align 1
  %30 = add nuw nsw i64 %.114.i.i.i22, 1
  %31 = icmp ult i64 %30, %.sroa.12.0
  br i1 %31, label %.lr.ph15.i.i.i21, label %.preheader.i.i.i19, !llvm.loop !111

rl_m_append__VectorTint8_tT_int8_t.exit.i:        ; preds = %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, %.preheader.i.i.i19
  %.sroa.22.2 = phi i64 [ %14, %.preheader.i.i.i19 ], [ %.sroa.22.1, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i ]
  %.sroa.026.2 = phi ptr [ %15, %.preheader.i.i.i19 ], [ %.sroa.026.1, %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i ]
  %32 = getelementptr i8, ptr %.sroa.026.2, i64 %.sroa.12.0
  %33 = load i8, ptr %10, align 1
  store i8 %33, ptr %32, align 1
  %34 = load i64, ptr %3, align 8
  %35 = icmp slt i64 %11, %34
  br i1 %35, label %rl_m_get__VectorTint8_tT_int64_t_r_int8_tRef.exit.i, label %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit

rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit:  ; preds = %rl_m_append__VectorTint8_tT_int8_t.exit.i, %vector.ph
  %.sroa.22.3 = phi i64 [ 4, %vector.ph ], [ %.sroa.22.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ]
  %.sroa.12.1 = phi i64 [ 0, %vector.ph ], [ %11, %rl_m_append__VectorTint8_tT_int8_t.exit.i ]
  %.sroa.026.3 = phi ptr [ %6, %vector.ph ], [ %.sroa.026.2, %rl_m_append__VectorTint8_tT_int8_t.exit.i ]
  %36 = load i64, ptr %4, align 8
  %.not3.i.i = icmp eq i64 %36, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %37

37:                                               ; preds = %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit
  %38 = load ptr, ptr %2, align 8
  call void @free(ptr %38)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %rl_m_assign__VectorTint8_tT_VectorTint8_tT.exit, %37
  store ptr %.sroa.026.3, ptr %0, align 1
  %.sroa.0.sroa.2.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 8
  store i64 %.sroa.12.1, ptr %.sroa.0.sroa.2.0..sroa_idx, align 1
  %.sroa.0.sroa.3.0..sroa_idx = getelementptr inbounds i8, ptr %0, i64 16
  store i64 %.sroa.22.3, ptr %.sroa.0.sroa.3.0..sroa_idx, align 1
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
  %6 = alloca ptr, align 8
  store ptr @str_16, ptr %6, align 8
  call void @rl_s__strlit_r_String(ptr nonnull %5, ptr nonnull %6)
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr %1, ptr nonnull readonly %5)
  %7 = getelementptr inbounds i8, ptr %5, i64 16
  %8 = load i64, ptr %7, align 8
  %.not3.i.i = icmp eq i64 %8, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %9

9:                                                ; preds = %4
  %10 = load ptr, ptr %5, align 8
  tail call void @free(ptr %10)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %4, %9
  %.pr.i = load i64, ptr %3, align 8
  %11 = icmp sgt i64 %.pr.i, -1
  br i1 %11, label %.lr.ph.i, label %._crit_edge.i

.lr.ph.i:                                         ; preds = %rl_m_drop__String.exit
  %12 = getelementptr i8, ptr %2, i64 8
  %13 = load i64, ptr %12, align 8
  %14 = icmp slt i64 %.pr.i, %13
  br i1 %14, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i, label %._crit_edge

._crit_edge.i:                                    ; preds = %rl_m_drop__String.exit
  %15 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

._crit_edge:                                      ; preds = %23, %.lr.ph.i
  %16 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit.i:      ; preds = %.lr.ph.i, %23
  %17 = phi i64 [ %25, %23 ], [ %13, %.lr.ph.i ]
  %18 = phi i64 [ %24, %23 ], [ %.pr.i, %.lr.ph.i ]
  %19 = load ptr, ptr %2, align 8
  %20 = getelementptr i8, ptr %19, i64 %18
  %21 = load i8, ptr %20, align 1
  switch i8 %21, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge [
    i8 32, label %rl_is_space__int8_t_r_bool.exit.thread.i
    i8 10, label %rl_is_space__int8_t_r_bool.exit.thread.i
    i8 0, label %rl_is_space__int8_t_r_bool.exit.thread.i
  ]

rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge: ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i
  %.pre = add nsw i64 %17, -1
  br label %rl__consume_space__String_int64_t.exit

rl_is_space__int8_t_r_bool.exit.thread.i:         ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i, %rl_m_get__String_int64_t_r_int8_tRef.exit.i, %rl_m_get__String_int64_t_r_int8_tRef.exit.i
  %22 = add nsw i64 %17, -1
  %.not9.i = icmp ult i64 %18, %22
  br i1 %.not9.i, label %23, label %rl__consume_space__String_int64_t.exit

23:                                               ; preds = %rl_is_space__int8_t_r_bool.exit.thread.i
  %24 = add nuw nsw i64 %18, 1
  store i64 %24, ptr %3, align 8
  %25 = load i64, ptr %12, align 8
  %26 = icmp slt i64 %24, %25
  br i1 %26, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i, label %._crit_edge

rl__consume_space__String_int64_t.exit:           ; preds = %rl_is_space__int8_t_r_bool.exit.thread.i, %rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge
  %.pre-phi = phi i64 [ %.pre, %rl_m_get__String_int64_t_r_int8_tRef.exit.i.rl__consume_space__String_int64_t.exit_crit_edge ], [ %22, %rl_is_space__int8_t_r_bool.exit.thread.i ]
  %.not.i = icmp slt i64 %18, %.pre-phi
  br i1 %.not.i, label %.lr.ph.preheader.i, label %common.ret

.lr.ph.preheader.i:                               ; preds = %rl__consume_space__String_int64_t.exit
  %27 = icmp sgt i64 %18, -1
  br i1 %27, label %.lr.ph.i6.preheader, label %32

.lr.ph.i6.preheader:                              ; preds = %.lr.ph.preheader.i
  %28 = icmp slt i64 %18, %17
  br i1 %28, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i7, label %34

rl_m_substring_matches__String_strlit_int64_t_r_bool.exit: ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i7
  %29 = add nuw nsw i64 %18, 1
  store i64 %29, ptr %3, align 8
  %30 = load i64, ptr %12, align 8
  %31 = add i64 %30, -2
  %.not543 = icmp eq i64 %18, %31
  br i1 %.not543, label %common.ret, label %.lr.ph

32:                                               ; preds = %.lr.ph.preheader.i
  %33 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

34:                                               ; preds = %.lr.ph.i6.preheader
  %35 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit.i7:     ; preds = %.lr.ph.i6.preheader
  %.not7.i = icmp eq i8 %21, 34
  br i1 %.not7.i, label %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit, label %common.ret

common.ret:                                       ; preds = %.backedge, %51, %rl__consume_space__String_int64_t.exit, %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit.i7, %142
  %.sink = phi i8 [ 1, %142 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i7 ], [ 0, %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit ], [ 0, %rl__consume_space__String_int64_t.exit ], [ 0, %51 ], [ 0, %.backedge ]
  store i8 %.sink, ptr %0, align 1
  ret void

.lr.ph:                                           ; preds = %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit
  %36 = getelementptr i8, ptr %1, i64 8
  %37 = getelementptr i8, ptr %1, i64 16
  br label %38

38:                                               ; preds = %.lr.ph, %.backedge
  %39 = phi i64 [ %30, %.lr.ph ], [ %99, %.backedge ]
  %40 = phi i64 [ %29, %.lr.ph ], [ %storemerge, %.backedge ]
  %41 = icmp sgt i64 %40, -1
  br i1 %41, label %44, label %42

42:                                               ; preds = %38
  %43 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

44:                                               ; preds = %38
  %45 = icmp slt i64 %40, %39
  br i1 %45, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %46

46:                                               ; preds = %44
  %47 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %44
  %48 = load ptr, ptr %2, align 8
  %49 = getelementptr i8, ptr %48, i64 %40
  %50 = load i8, ptr %49, align 1
  switch i8 %50, label %101 [
    i8 34, label %142
    i8 92, label %51
  ]

51:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %52 = add nuw nsw i64 %40, 1
  store i64 %52, ptr %3, align 8
  %53 = load i64, ptr %12, align 8
  %54 = add i64 %53, -2
  %55 = icmp eq i64 %40, %54
  br i1 %55, label %common.ret, label %56

56:                                               ; preds = %51
  %57 = icmp slt i64 %52, %53
  br i1 %57, label %rl_m_get__String_int64_t_r_int8_tRef.exit9, label %58

58:                                               ; preds = %56
  %59 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit9:       ; preds = %56
  %60 = load ptr, ptr %2, align 8
  %61 = getelementptr i8, ptr %60, i64 %52
  %62 = load i8, ptr %61, align 1
  %63 = icmp eq i8 %62, 34
  br i1 %63, label %64, label %101

64:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit9
  %65 = load i64, ptr %36, align 8
  %66 = icmp sgt i64 %65, 0
  br i1 %66, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i, label %67

67:                                               ; preds = %64
  %68 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i:     ; preds = %64
  %69 = load ptr, ptr %1, align 8
  %70 = getelementptr i8, ptr %69, i64 %65
  %71 = getelementptr i8, ptr %70, i64 -1
  store i8 34, ptr %71, align 1
  %72 = load i64, ptr %36, align 8
  %73 = add i64 %72, 1
  %74 = load i64, ptr %37, align 8
  %75 = icmp sgt i64 %74, %73
  br i1 %75, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i, label %76

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i
  %.pre2.i.i = load ptr, ptr %1, align 8
  br label %rl_m_append__String_int8_t.exit

76:                                               ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i
  %77 = shl i64 %73, 1
  %78 = tail call ptr @malloc(i64 %77)
  %79 = ptrtoint ptr %78 to i64
  %80 = icmp sgt i64 %77, 0
  br i1 %80, label %.lr.ph.preheader.i.i.i, label %.preheader12.i.i.i

.lr.ph.preheader.i.i.i:                           ; preds = %76
  tail call void @llvm.memset.p0.i64(ptr align 1 %78, i8 0, i64 %77, i1 false)
  br label %.preheader12.i.i.i

.preheader12.i.i.i:                               ; preds = %.lr.ph.preheader.i.i.i, %76
  %81 = icmp sgt i64 %72, 0
  %.pre.i.i.i = load ptr, ptr %1, align 8
  br i1 %81, label %iter.check109, label %.preheader.i.i.i

iter.check109:                                    ; preds = %.preheader12.i.i.i
  %.pre.i.i.i104 = ptrtoint ptr %.pre.i.i.i to i64
  %min.iters.check107 = icmp ult i64 %72, 8
  %82 = sub i64 %79, %.pre.i.i.i104
  %diff.check105 = icmp ult i64 %82, 32
  %or.cond = select i1 %min.iters.check107, i1 true, i1 %diff.check105
  br i1 %or.cond, label %.lr.ph15.i.i.i.preheader, label %vector.main.loop.iter.check111

vector.main.loop.iter.check111:                   ; preds = %iter.check109
  %min.iters.check110 = icmp ult i64 %72, 32
  br i1 %min.iters.check110, label %vec.epilog.ph123, label %vector.ph112

vector.ph112:                                     ; preds = %vector.main.loop.iter.check111
  %n.vec114 = and i64 %72, 9223372036854775776
  br label %vector.body115

vector.body115:                                   ; preds = %vector.body115, %vector.ph112
  %index116 = phi i64 [ 0, %vector.ph112 ], [ %index.next119, %vector.body115 ]
  %83 = getelementptr i8, ptr %78, i64 %index116
  %84 = getelementptr i8, ptr %.pre.i.i.i, i64 %index116
  %85 = getelementptr i8, ptr %84, i64 16
  %wide.load117 = load <16 x i8>, ptr %84, align 1
  %wide.load118 = load <16 x i8>, ptr %85, align 1
  %86 = getelementptr i8, ptr %83, i64 16
  store <16 x i8> %wide.load117, ptr %83, align 1
  store <16 x i8> %wide.load118, ptr %86, align 1
  %index.next119 = add nuw i64 %index116, 32
  %87 = icmp eq i64 %index.next119, %n.vec114
  br i1 %87, label %middle.block106, label %vector.body115, !llvm.loop !112

middle.block106:                                  ; preds = %vector.body115
  %cmp.n120 = icmp eq i64 %72, %n.vec114
  br i1 %cmp.n120, label %.preheader.i.i.i, label %vec.epilog.iter.check124

vec.epilog.iter.check124:                         ; preds = %middle.block106
  %n.vec.remaining125 = and i64 %72, 24
  %min.epilog.iters.check126 = icmp eq i64 %n.vec.remaining125, 0
  br i1 %min.epilog.iters.check126, label %.lr.ph15.i.i.i.preheader, label %vec.epilog.ph123

.lr.ph15.i.i.i.preheader:                         ; preds = %vec.epilog.middle.block121, %iter.check109, %vec.epilog.iter.check124
  %.114.i.i.i.ph = phi i64 [ 0, %iter.check109 ], [ %n.vec114, %vec.epilog.iter.check124 ], [ %n.vec129, %vec.epilog.middle.block121 ]
  br label %.lr.ph15.i.i.i

vec.epilog.ph123:                                 ; preds = %vector.main.loop.iter.check111, %vec.epilog.iter.check124
  %vec.epilog.resume.val127 = phi i64 [ %n.vec114, %vec.epilog.iter.check124 ], [ 0, %vector.main.loop.iter.check111 ]
  %n.vec129 = and i64 %72, 9223372036854775800
  br label %vec.epilog.vector.body131

vec.epilog.vector.body131:                        ; preds = %vec.epilog.vector.body131, %vec.epilog.ph123
  %index132 = phi i64 [ %vec.epilog.resume.val127, %vec.epilog.ph123 ], [ %index.next134, %vec.epilog.vector.body131 ]
  %88 = getelementptr i8, ptr %78, i64 %index132
  %89 = getelementptr i8, ptr %.pre.i.i.i, i64 %index132
  %wide.load133 = load <8 x i8>, ptr %89, align 1
  store <8 x i8> %wide.load133, ptr %88, align 1
  %index.next134 = add nuw i64 %index132, 8
  %90 = icmp eq i64 %index.next134, %n.vec129
  br i1 %90, label %vec.epilog.middle.block121, label %vec.epilog.vector.body131, !llvm.loop !113

vec.epilog.middle.block121:                       ; preds = %vec.epilog.vector.body131
  %cmp.n135 = icmp eq i64 %72, %n.vec129
  br i1 %cmp.n135, label %.preheader.i.i.i, label %.lr.ph15.i.i.i.preheader

.preheader.i.i.i:                                 ; preds = %.lr.ph15.i.i.i, %middle.block106, %vec.epilog.middle.block121, %.preheader12.i.i.i
  tail call void @free(ptr %.pre.i.i.i)
  store i64 %77, ptr %37, align 8
  store ptr %78, ptr %1, align 8
  %.pre.i.i = load i64, ptr %36, align 8
  br label %rl_m_append__String_int8_t.exit

.lr.ph15.i.i.i:                                   ; preds = %.lr.ph15.i.i.i.preheader, %.lr.ph15.i.i.i
  %.114.i.i.i = phi i64 [ %94, %.lr.ph15.i.i.i ], [ %.114.i.i.i.ph, %.lr.ph15.i.i.i.preheader ]
  %91 = getelementptr i8, ptr %78, i64 %.114.i.i.i
  %92 = getelementptr i8, ptr %.pre.i.i.i, i64 %.114.i.i.i
  %93 = load i8, ptr %92, align 1
  store i8 %93, ptr %91, align 1
  %94 = add nuw nsw i64 %.114.i.i.i, 1
  %95 = icmp slt i64 %94, %72
  br i1 %95, label %.lr.ph15.i.i.i, label %.preheader.i.i.i, !llvm.loop !114

rl_m_append__String_int8_t.exit:                  ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i, %.preheader.i.i.i
  %96 = phi ptr [ %.pre2.i.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i ], [ %78, %.preheader.i.i.i ]
  %97 = phi i64 [ %72, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i ], [ %.pre.i.i, %.preheader.i.i.i ]
  %98 = getelementptr i8, ptr %96, i64 %97
  br label %.backedge

.backedge:                                        ; preds = %rl_m_append__String_int8_t.exit, %rl_m_append__String_int8_t.exit21
  %.sink76 = phi ptr [ %98, %rl_m_append__String_int8_t.exit ], [ %141, %rl_m_append__String_int8_t.exit21 ]
  store i8 0, ptr %.sink76, align 1
  %storemerge57.in = load i64, ptr %36, align 8
  %storemerge57 = add i64 %storemerge57.in, 1
  store i64 %storemerge57, ptr %36, align 8
  %storemerge.in = load i64, ptr %3, align 8
  %storemerge = add i64 %storemerge.in, 1
  store i64 %storemerge, ptr %3, align 8
  %99 = load i64, ptr %12, align 8
  %100 = add i64 %99, -2
  %.not5 = icmp eq i64 %storemerge.in, %100
  br i1 %.not5, label %common.ret, label %38

101:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit9, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %102 = phi i8 [ %50, %rl_m_get__String_int64_t_r_int8_tRef.exit ], [ %62, %rl_m_get__String_int64_t_r_int8_tRef.exit9 ]
  %103 = phi i64 [ %39, %rl_m_get__String_int64_t_r_int8_tRef.exit ], [ %53, %rl_m_get__String_int64_t_r_int8_tRef.exit9 ]
  %104 = phi i64 [ %40, %rl_m_get__String_int64_t_r_int8_tRef.exit ], [ %52, %rl_m_get__String_int64_t_r_int8_tRef.exit9 ]
  %105 = icmp ult i64 %104, %103
  br i1 %105, label %rl_m_get__String_int64_t_r_int8_tRef.exit10, label %106

106:                                              ; preds = %101
  %107 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit10:      ; preds = %101
  %108 = load i64, ptr %36, align 8
  %109 = icmp sgt i64 %108, 0
  br i1 %109, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i11, label %110

110:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit10
  %111 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i11:   ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit10
  %112 = load ptr, ptr %1, align 8
  %113 = getelementptr i8, ptr %112, i64 %108
  %114 = getelementptr i8, ptr %113, i64 -1
  store i8 %102, ptr %114, align 1
  %115 = load i64, ptr %36, align 8
  %116 = add i64 %115, 1
  %117 = load i64, ptr %37, align 8
  %118 = icmp sgt i64 %117, %116
  br i1 %118, label %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i19, label %119

.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i19: ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i11
  %.pre2.i.i20 = load ptr, ptr %1, align 8
  br label %rl_m_append__String_int8_t.exit21

119:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i11
  %120 = shl i64 %116, 1
  %121 = tail call ptr @malloc(i64 %120)
  %122 = ptrtoint ptr %121 to i64
  %123 = icmp sgt i64 %120, 0
  br i1 %123, label %.lr.ph.preheader.i.i.i18, label %.preheader12.i.i.i12

.lr.ph.preheader.i.i.i18:                         ; preds = %119
  tail call void @llvm.memset.p0.i64(ptr align 1 %121, i8 0, i64 %120, i1 false)
  br label %.preheader12.i.i.i12

.preheader12.i.i.i12:                             ; preds = %.lr.ph.preheader.i.i.i18, %119
  %124 = icmp sgt i64 %115, 0
  %.pre.i.i.i13 = load ptr, ptr %1, align 8
  br i1 %124, label %iter.check, label %.preheader.i.i.i14

iter.check:                                       ; preds = %.preheader12.i.i.i12
  %.pre.i.i.i1394 = ptrtoint ptr %.pre.i.i.i13 to i64
  %min.iters.check = icmp ult i64 %115, 8
  %125 = sub i64 %122, %.pre.i.i.i1394
  %diff.check = icmp ult i64 %125, 32
  %or.cond136 = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond136, label %.lr.ph15.i.i.i16.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check95 = icmp ult i64 %115, 32
  br i1 %min.iters.check95, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %115, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %126 = getelementptr i8, ptr %121, i64 %index
  %127 = getelementptr i8, ptr %.pre.i.i.i13, i64 %index
  %128 = getelementptr i8, ptr %127, i64 16
  %wide.load = load <16 x i8>, ptr %127, align 1
  %wide.load96 = load <16 x i8>, ptr %128, align 1
  %129 = getelementptr i8, ptr %126, i64 16
  store <16 x i8> %wide.load, ptr %126, align 1
  store <16 x i8> %wide.load96, ptr %129, align 1
  %index.next = add nuw i64 %index, 32
  %130 = icmp eq i64 %index.next, %n.vec
  br i1 %130, label %middle.block, label %vector.body, !llvm.loop !115

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %115, %n.vec
  br i1 %cmp.n, label %.preheader.i.i.i14, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %115, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.i16.preheader, label %vec.epilog.ph

.lr.ph15.i.i.i16.preheader:                       ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.i17.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec98, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i.i16

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec98 = and i64 %115, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index99 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next101, %vec.epilog.vector.body ]
  %131 = getelementptr i8, ptr %121, i64 %index99
  %132 = getelementptr i8, ptr %.pre.i.i.i13, i64 %index99
  %wide.load100 = load <8 x i8>, ptr %132, align 1
  store <8 x i8> %wide.load100, ptr %131, align 1
  %index.next101 = add nuw i64 %index99, 8
  %133 = icmp eq i64 %index.next101, %n.vec98
  br i1 %133, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !116

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n102 = icmp eq i64 %115, %n.vec98
  br i1 %cmp.n102, label %.preheader.i.i.i14, label %.lr.ph15.i.i.i16.preheader

.preheader.i.i.i14:                               ; preds = %.lr.ph15.i.i.i16, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i.i12
  tail call void @free(ptr %.pre.i.i.i13)
  store i64 %120, ptr %37, align 8
  store ptr %121, ptr %1, align 8
  %.pre.i.i15 = load i64, ptr %36, align 8
  br label %rl_m_append__String_int8_t.exit21

.lr.ph15.i.i.i16:                                 ; preds = %.lr.ph15.i.i.i16.preheader, %.lr.ph15.i.i.i16
  %.114.i.i.i17 = phi i64 [ %137, %.lr.ph15.i.i.i16 ], [ %.114.i.i.i17.ph, %.lr.ph15.i.i.i16.preheader ]
  %134 = getelementptr i8, ptr %121, i64 %.114.i.i.i17
  %135 = getelementptr i8, ptr %.pre.i.i.i13, i64 %.114.i.i.i17
  %136 = load i8, ptr %135, align 1
  store i8 %136, ptr %134, align 1
  %137 = add nuw nsw i64 %.114.i.i.i17, 1
  %138 = icmp slt i64 %137, %115
  br i1 %138, label %.lr.ph15.i.i.i16, label %.preheader.i.i.i14, !llvm.loop !117

rl_m_append__String_int8_t.exit21:                ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i19, %.preheader.i.i.i14
  %139 = phi ptr [ %.pre2.i.i20, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i19 ], [ %121, %.preheader.i.i.i14 ]
  %140 = phi i64 [ %115, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i19 ], [ %.pre.i.i15, %.preheader.i.i.i14 ]
  %141 = getelementptr i8, ptr %139, i64 %140
  br label %.backedge

142:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %143 = add nuw nsw i64 %40, 1
  store i64 %143, ptr %3, align 8
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
  %9 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

._crit_edge:                                      ; preds = %17, %.lr.ph.i
  %10 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %.not9.i = icmp ult i64 %12, %16
  br i1 %.not9.i, label %17, label %rl__consume_space__String_int64_t.exit

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
  %.not7.i.1 = icmp eq i8 %26, 114
  br i1 %.not7.i.1, label %.lr.ph.i4.2, label %common.ret

.lr.ph.i4.2:                                      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.1
  %27 = add nuw nsw i64 %12, 2
  %28 = icmp ult i64 %27, %11
  br i1 %28, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.2, label %37

rl_m_get__String_int64_t_r_int8_tRef.exit.i5.2:   ; preds = %.lr.ph.i4.2
  %29 = getelementptr i8, ptr %13, i64 %27
  %30 = load i8, ptr %29, align 1
  %.not7.i.2 = icmp eq i8 %30, 117
  br i1 %.not7.i.2, label %.lr.ph.i4.3, label %common.ret

.lr.ph.i4.3:                                      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.2
  %31 = add nuw nsw i64 %12, 3
  %32 = icmp ult i64 %31, %11
  br i1 %32, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i5.3, label %37

rl_m_get__String_int64_t_r_int8_tRef.exit.i5.3:   ; preds = %.lr.ph.i4.3
  %33 = getelementptr i8, ptr %13, i64 %31
  %34 = load i8, ptr %33, align 1
  %.not7.i.3 = icmp eq i8 %34, 101
  br i1 %.not7.i.3, label %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit, label %common.ret

35:                                               ; preds = %.lr.ph.preheader.i
  %36 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

37:                                               ; preds = %.lr.ph.i4.3, %.lr.ph.i4.2, %.lr.ph.i4.1, %.lr.ph.i4.preheader
  %38 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %.not7.i14.1 = icmp eq i8 %42, 97
  br i1 %.not7.i14.1, label %.lr.ph.preheader.i10.2, label %common.ret

.lr.ph.preheader.i10.2:                           ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.1
  %43 = add nuw nsw i64 %12, 2
  %44 = icmp ult i64 %43, %11
  br i1 %44, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.2, label %55

rl_m_get__String_int64_t_r_int8_tRef.exit.i13.2:  ; preds = %.lr.ph.preheader.i10.2
  %45 = getelementptr i8, ptr %13, i64 %43
  %46 = load i8, ptr %45, align 1
  %.not7.i14.2 = icmp eq i8 %46, 108
  br i1 %.not7.i14.2, label %.lr.ph.preheader.i10.3, label %common.ret

.lr.ph.preheader.i10.3:                           ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.2
  %47 = add nuw nsw i64 %12, 3
  %48 = icmp ult i64 %47, %11
  br i1 %48, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.3, label %55

rl_m_get__String_int64_t_r_int8_tRef.exit.i13.3:  ; preds = %.lr.ph.preheader.i10.3
  %49 = getelementptr i8, ptr %13, i64 %47
  %50 = load i8, ptr %49, align 1
  %.not7.i14.3 = icmp eq i8 %50, 115
  br i1 %.not7.i14.3, label %.lr.ph.preheader.i10.4, label %common.ret

.lr.ph.preheader.i10.4:                           ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.3
  %51 = add nuw nsw i64 %12, 4
  %52 = icmp ult i64 %51, %11
  br i1 %52, label %rl_m_get__String_int64_t_r_int8_tRef.exit.i13.4, label %55

rl_m_get__String_int64_t_r_int8_tRef.exit.i13.4:  ; preds = %.lr.ph.preheader.i10.4
  %53 = getelementptr i8, ptr %13, i64 %51
  %54 = load i8, ptr %53, align 1
  %.not7.i14.4 = icmp eq i8 %54, 101
  br i1 %.not7.i14.4, label %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit, label %common.ret

55:                                               ; preds = %.lr.ph.preheader.i10.4, %.lr.ph.preheader.i10.3, %.lr.ph.preheader.i10.2, %.lr.ph.preheader.i10.1
  %56 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %3 = alloca i64, align 8
  %4 = alloca %String, align 8
  %5 = alloca %String, align 8
  %6 = alloca ptr, align 8
  %7 = alloca %String, align 8
  %8 = alloca %String, align 8
  %9 = alloca %String, align 8
  %10 = alloca ptr, align 8
  %11 = alloca %String, align 8
  %12 = alloca %String, align 8
  %13 = alloca i64, align 8
  %14 = alloca %String, align 8
  %15 = alloca %String, align 8
  %16 = alloca ptr, align 8
  %17 = alloca %String, align 8
  %18 = alloca %String, align 8
  %19 = alloca %String, align 8
  %20 = alloca ptr, align 8
  %21 = alloca i8, align 1
  %22 = alloca %Dict, align 8
  %23 = alloca %Vector.1, align 8
  %24 = alloca %Vector.1, align 8
  %25 = alloca ptr, align 8
  %26 = alloca i8, align 1
  %27 = alloca %String, align 8
  %28 = alloca %String, align 8
  %29 = alloca i64, align 8
  %30 = alloca %String, align 8
  %31 = alloca %String, align 8
  %32 = alloca ptr, align 8
  %33 = alloca %String, align 8
  %34 = alloca %String, align 8
  %35 = alloca %String, align 8
  %36 = alloca ptr, align 8
  %37 = alloca i64, align 8
  %38 = alloca i8, align 1
  %39 = alloca i64, align 8
  %40 = alloca i64, align 8
  %41 = alloca %Dict, align 8
  %42 = getelementptr inbounds i8, ptr %41, i64 16
  store i64 4, ptr %42, align 8
  %43 = getelementptr inbounds i8, ptr %41, i64 8
  store i64 0, ptr %43, align 8
  %44 = getelementptr inbounds i8, ptr %41, i64 24
  store double 7.500000e-01, ptr %44, align 8
  %45 = tail call dereferenceable_or_null(128) ptr @malloc(i64 128)
  store ptr %45, ptr %41, align 8
  store i8 0, ptr %45, align 1
  %46 = getelementptr i8, ptr %45, i64 32
  store i8 0, ptr %46, align 1
  %47 = getelementptr i8, ptr %45, i64 64
  store i8 0, ptr %47, align 1
  %48 = getelementptr i8, ptr %45, i64 96
  store i8 0, ptr %48, align 1
  br label %rl_m_init__DictTint64_tTint64_tT.exit

.preheader120:                                    ; preds = %rl_m_init__DictTint64_tTint64_tT.exit
  %49 = getelementptr inbounds i8, ptr %35, i64 16
  %50 = getelementptr inbounds i8, ptr %35, i64 8
  %51 = getelementptr inbounds i8, ptr %34, i64 16
  %52 = getelementptr inbounds i8, ptr %34, i64 8
  %53 = getelementptr inbounds i8, ptr %33, i64 16
  %54 = getelementptr inbounds i8, ptr %33, i64 8
  %55 = getelementptr inbounds i8, ptr %31, i64 16
  %56 = getelementptr inbounds i8, ptr %31, i64 8
  %57 = getelementptr inbounds i8, ptr %30, i64 16
  %58 = getelementptr inbounds i8, ptr %30, i64 8
  %59 = getelementptr inbounds i8, ptr %28, i64 16
  %60 = getelementptr inbounds i8, ptr %28, i64 8
  %61 = getelementptr inbounds i8, ptr %27, i64 16
  %62 = getelementptr inbounds i8, ptr %27, i64 8
  %63 = load i64, ptr %43, align 8
  %64 = icmp eq i64 %63, 0
  br i1 %64, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread.us.preheader, label %.preheader120.split

rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread.us.preheader: ; preds = %.preheader120
  store i64 40, ptr %37, align 8
  br label %.split

rl_m_init__DictTint64_tTint64_tT.exit:            ; preds = %.lr.ph.i, %rl_m_init__DictTint64_tTint64_tT.exit
  %.0111123 = phi i64 [ %65, %rl_m_init__DictTint64_tTint64_tT.exit ], [ 0, %.lr.ph.i ]
  store i64 %.0111123, ptr %40, align 8
  %65 = add nuw nsw i64 %.0111123, 1
  %66 = mul i64 %.0111123, %.0111123
  store i64 %66, ptr %39, align 8
  call void @rl_m_insert__DictTint64_tTint64_tT_int64_t_int64_t_r_bool(ptr nonnull %38, ptr nonnull %41, ptr nonnull %40, ptr nonnull %39)
  %.not = icmp eq i64 %65, 50
  br i1 %.not, label %.preheader120, label %rl_m_init__DictTint64_tTint64_tT.exit

.preheader120.split:                              ; preds = %.preheader120, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread
  %67 = phi i64 [ %154, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread ], [ %63, %.preheader120 ]
  %.0113124 = phi i64 [ %68, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread ], [ 0, %.preheader120 ]
  %68 = add nuw nsw i64 %.0113124, 1
  %69 = mul nuw nsw i64 %.0113124, 10
  store i64 %69, ptr %37, align 8
  %70 = icmp eq i64 %67, 0
  br i1 %70, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %71

71:                                               ; preds = %.preheader120.split
  %72 = lshr i64 %69, 33
  %73 = xor i64 %72, %69
  %74 = mul i64 %73, 1099511628211
  %75 = lshr i64 %74, 33
  %76 = xor i64 %75, %74
  %77 = mul i64 %76, 16777619
  %78 = lshr i64 %77, 33
  %.masked.i.i.i.i = and i64 %77, 9223372036854775807
  %79 = xor i64 %.masked.i.i.i.i, %78
  %80 = load i64, ptr %42, align 8
  %.not23.i = icmp sgt i64 %80, 0
  br i1 %.not23.i, label %.lr.ph.i8, label %._crit_edge.i

.lr.ph.i8:                                        ; preds = %71
  %81 = load ptr, ptr %41, align 8
  br label %83

._crit_edge.i:                                    ; preds = %71, %99
  %82 = call i32 @puts(ptr nonnull dereferenceable(1) @str_3)
  call void @llvm.trap()
  unreachable

83:                                               ; preds = %99, %.lr.ph.i8
  %.pn.i = phi i64 [ %79, %.lr.ph.i8 ], [ %100, %99 ]
  %.01125.i = phi i64 [ 0, %.lr.ph.i8 ], [ %84, %99 ]
  %.026.i = srem i64 %.pn.i, %80
  %84 = add nuw nsw i64 %.01125.i, 1
  %85 = getelementptr %Entry, ptr %81, i64 %.026.i
  %86 = load i8, ptr %85, align 1
  %87 = icmp eq i8 %86, 0
  br i1 %87, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %88

88:                                               ; preds = %83
  %89 = getelementptr i8, ptr %85, i64 8
  %90 = load i64, ptr %89, align 8
  %91 = icmp eq i64 %90, %79
  br i1 %91, label %92, label %.thread.i

92:                                               ; preds = %88
  %93 = getelementptr i8, ptr %85, i64 16
  %.val.i16.i = load i64, ptr %93, align 8
  %.not21.i = icmp eq i64 %.val.i16.i, %69
  br i1 %.not21.i, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit, label %.thread.i

.thread.i:                                        ; preds = %92, %88
  %94 = add i64 %.026.i, %80
  %95 = srem i64 %90, %80
  %96 = sub i64 %94, %95
  %97 = srem i64 %96, %80
  %98 = icmp slt i64 %97, %.01125.i
  br i1 %98, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %99

99:                                               ; preds = %.thread.i
  %100 = add i64 %.026.i, 1
  %.not.i = icmp slt i64 %84, %80
  br i1 %.not.i, label %83, label %._crit_edge.i

rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit: ; preds = %92
  store ptr @str_18, ptr %36, align 8
  call void @rl_s__strlit_r_String(ptr nonnull %35, ptr nonnull %36)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %34, ptr nonnull %37)
  call void @rl_m_add__String_String_r_String(ptr nonnull %33, ptr nonnull %35, ptr nonnull %34)
  store ptr @str_19, ptr %32, align 8
  call void @rl_s__strlit_r_String(ptr nonnull %31, ptr nonnull %32)
  call void @rl_m_add__String_String_r_String(ptr nonnull %30, ptr nonnull %33, ptr nonnull %31)
  call void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr nonnull %29, ptr nonnull %41, ptr nonnull %37)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %28, ptr nonnull %29)
  call void @rl_m_add__String_String_r_String(ptr nonnull %27, ptr nonnull %30, ptr nonnull %28)
  call void @rl_print_string__String(ptr nonnull %27)
  %101 = load i64, ptr %49, align 8
  %.not3.i.i = icmp eq i64 %101, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %102

102:                                              ; preds = %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit
  %103 = load ptr, ptr %35, align 8
  call void @free(ptr %103)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit, %102
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %50, i8 0, i64 16, i1 false)
  %104 = load i64, ptr %51, align 8
  %.not3.i.i9 = icmp eq i64 %104, 0
  br i1 %.not3.i.i9, label %rl_m_drop__String.exit10, label %105

105:                                              ; preds = %rl_m_drop__String.exit
  %106 = load ptr, ptr %34, align 8
  call void @free(ptr %106)
  br label %rl_m_drop__String.exit10

rl_m_drop__String.exit10:                         ; preds = %rl_m_drop__String.exit, %105
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %52, i8 0, i64 16, i1 false)
  %107 = load i64, ptr %53, align 8
  %.not3.i.i11 = icmp eq i64 %107, 0
  br i1 %.not3.i.i11, label %rl_m_drop__String.exit12, label %108

108:                                              ; preds = %rl_m_drop__String.exit10
  %109 = load ptr, ptr %33, align 8
  call void @free(ptr %109)
  br label %rl_m_drop__String.exit12

rl_m_drop__String.exit12:                         ; preds = %rl_m_drop__String.exit10, %108
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %54, i8 0, i64 16, i1 false)
  %110 = load i64, ptr %55, align 8
  %.not3.i.i13 = icmp eq i64 %110, 0
  br i1 %.not3.i.i13, label %rl_m_drop__String.exit14, label %111

111:                                              ; preds = %rl_m_drop__String.exit12
  %112 = load ptr, ptr %31, align 8
  call void @free(ptr %112)
  br label %rl_m_drop__String.exit14

rl_m_drop__String.exit14:                         ; preds = %rl_m_drop__String.exit12, %111
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %56, i8 0, i64 16, i1 false)
  %113 = load i64, ptr %57, align 8
  %.not3.i.i15 = icmp eq i64 %113, 0
  br i1 %.not3.i.i15, label %rl_m_drop__String.exit16, label %114

114:                                              ; preds = %rl_m_drop__String.exit14
  %115 = load ptr, ptr %30, align 8
  call void @free(ptr %115)
  br label %rl_m_drop__String.exit16

rl_m_drop__String.exit16:                         ; preds = %rl_m_drop__String.exit14, %114
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %58, i8 0, i64 16, i1 false)
  %116 = load i64, ptr %59, align 8
  %.not3.i.i17 = icmp eq i64 %116, 0
  br i1 %.not3.i.i17, label %rl_m_drop__String.exit18, label %117

117:                                              ; preds = %rl_m_drop__String.exit16
  %118 = load ptr, ptr %28, align 8
  call void @free(ptr %118)
  br label %rl_m_drop__String.exit18

rl_m_drop__String.exit18:                         ; preds = %rl_m_drop__String.exit16, %117
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %60, i8 0, i64 16, i1 false)
  %119 = load i64, ptr %61, align 8
  %.not3.i.i19 = icmp eq i64 %119, 0
  br i1 %.not3.i.i19, label %rl_m_drop__String.exit20, label %120

120:                                              ; preds = %rl_m_drop__String.exit18
  %121 = load ptr, ptr %27, align 8
  call void @free(ptr %121)
  br label %rl_m_drop__String.exit20

rl_m_drop__String.exit20:                         ; preds = %rl_m_drop__String.exit18, %120
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %62, i8 0, i64 16, i1 false)
  call void @rl_m_remove__DictTint64_tTint64_tT_int64_t_r_bool(ptr nonnull %26, ptr nonnull %41, ptr nonnull %37)
  %122 = load i64, ptr %43, align 8
  %123 = icmp eq i64 %122, 0
  br i1 %123, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %124

124:                                              ; preds = %rl_m_drop__String.exit20
  %.val.i.i21 = load i64, ptr %37, align 8
  %125 = lshr i64 %.val.i.i21, 33
  %126 = xor i64 %125, %.val.i.i21
  %127 = mul i64 %126, 1099511628211
  %128 = lshr i64 %127, 33
  %129 = xor i64 %128, %127
  %130 = mul i64 %129, 16777619
  %131 = lshr i64 %130, 33
  %.masked.i.i.i.i22 = and i64 %130, 9223372036854775807
  %132 = xor i64 %.masked.i.i.i.i22, %131
  %133 = load i64, ptr %42, align 8
  %.not23.i23 = icmp sgt i64 %133, 0
  br i1 %.not23.i23, label %.lr.ph.i25, label %._crit_edge.i24

.lr.ph.i25:                                       ; preds = %124
  %134 = load ptr, ptr %41, align 8
  br label %136

._crit_edge.i24:                                  ; preds = %124, %152
  %135 = call i32 @puts(ptr nonnull dereferenceable(1) @str_3)
  call void @llvm.trap()
  unreachable

136:                                              ; preds = %152, %.lr.ph.i25
  %.pn.i26 = phi i64 [ %132, %.lr.ph.i25 ], [ %153, %152 ]
  %.01125.i27 = phi i64 [ 0, %.lr.ph.i25 ], [ %137, %152 ]
  %.026.i28 = srem i64 %.pn.i26, %133
  %137 = add nuw nsw i64 %.01125.i27, 1
  %138 = getelementptr %Entry, ptr %134, i64 %.026.i28
  %139 = load i8, ptr %138, align 1
  %140 = icmp eq i8 %139, 0
  br i1 %140, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %141

141:                                              ; preds = %136
  %142 = getelementptr i8, ptr %138, i64 8
  %143 = load i64, ptr %142, align 8
  %144 = icmp eq i64 %143, %132
  br i1 %144, label %145, label %.thread.i29

145:                                              ; preds = %141
  %146 = getelementptr i8, ptr %138, i64 16
  %.val.i16.i32 = load i64, ptr %146, align 8
  %.not21.i33 = icmp eq i64 %.val.i16.i32, %.val.i.i21
  br i1 %.not21.i33, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34, label %.thread.i29

.thread.i29:                                      ; preds = %145, %141
  %147 = add i64 %.026.i28, %133
  %148 = srem i64 %143, %133
  %149 = sub i64 %147, %148
  %150 = srem i64 %149, %133
  %151 = icmp slt i64 %150, %.01125.i27
  br i1 %151, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %152

152:                                              ; preds = %.thread.i29
  %153 = add i64 %.026.i28, 1
  %.not.i30 = icmp slt i64 %137, %133
  br i1 %.not.i30, label %136, label %._crit_edge.i24

rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34: ; preds = %145
  store ptr @str_20, ptr %25, align 8
  call void @rl_print_string_lit__strlit(ptr nonnull %25)
  br label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread

rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread: ; preds = %83, %.thread.i, %136, %.thread.i29, %rl_m_drop__String.exit20, %.preheader120.split, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34
  %154 = phi i64 [ 0, %rl_m_drop__String.exit20 ], [ 0, %.preheader120.split ], [ %122, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34 ], [ %122, %.thread.i29 ], [ %122, %136 ], [ %67, %.thread.i ], [ %67, %83 ]
  %.not2 = icmp eq i64 %68, 5
  br i1 %.not2, label %.split, label %.preheader120.split, !llvm.loop !118

.split:                                           ; preds = %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread.us.preheader
  call void @rl_m_keys__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr nonnull %24, ptr nonnull %41)
  call void @rl_m_values__DictTint64_tTint64_tT_r_VectorTint64_tT(ptr nonnull %23, ptr nonnull %41)
  %155 = getelementptr inbounds i8, ptr %22, i64 16
  store i64 4, ptr %155, align 8
  %156 = getelementptr inbounds i8, ptr %22, i64 8
  store i64 0, ptr %156, align 8
  %157 = getelementptr inbounds i8, ptr %22, i64 24
  store double 7.500000e-01, ptr %157, align 8
  %158 = call dereferenceable_or_null(128) ptr @malloc(i64 128)
  store ptr %158, ptr %22, align 8
  store i8 0, ptr %158, align 1
  %159 = getelementptr i8, ptr %158, i64 32
  store i8 0, ptr %159, align 1
  %160 = getelementptr i8, ptr %158, i64 64
  store i8 0, ptr %160, align 1
  %161 = getelementptr i8, ptr %158, i64 96
  store i8 0, ptr %161, align 1
  %162 = getelementptr inbounds i8, ptr %24, i64 8
  %163 = load i64, ptr %162, align 8
  %.not3125 = icmp eq i64 %163, 0
  br i1 %.not3125, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %.split
  %164 = getelementptr inbounds i8, ptr %23, i64 8
  %165 = load i64, ptr %164, align 8
  %166 = load ptr, ptr %23, align 8
  %167 = load ptr, ptr %24, align 8
  br label %183

.lr.ph129:                                        ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39
  %168 = load ptr, ptr %24, align 8
  %169 = getelementptr inbounds i8, ptr %19, i64 16
  %170 = getelementptr inbounds i8, ptr %19, i64 8
  %171 = getelementptr inbounds i8, ptr %18, i64 16
  %172 = getelementptr inbounds i8, ptr %18, i64 8
  %173 = getelementptr inbounds i8, ptr %17, i64 16
  %174 = getelementptr inbounds i8, ptr %17, i64 8
  %175 = getelementptr inbounds i8, ptr %15, i64 16
  %176 = getelementptr inbounds i8, ptr %15, i64 8
  %177 = getelementptr inbounds i8, ptr %14, i64 16
  %178 = getelementptr inbounds i8, ptr %14, i64 8
  %179 = getelementptr inbounds i8, ptr %12, i64 16
  %180 = getelementptr inbounds i8, ptr %12, i64 8
  %181 = getelementptr inbounds i8, ptr %11, i64 16
  %182 = getelementptr inbounds i8, ptr %11, i64 8
  br label %194

183:                                              ; preds = %.lr.ph, %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39
  %.0112126 = phi i64 [ 0, %.lr.ph ], [ %184, %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39 ]
  %184 = add nuw i64 %.0112126, 1
  %185 = icmp slt i64 %.0112126, %165
  br i1 %185, label %188, label %186

186:                                              ; preds = %183
  %187 = call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  call void @llvm.trap()
  unreachable

188:                                              ; preds = %183
  %189 = icmp slt i64 %.0112126, %163
  br i1 %189, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39, label %190

190:                                              ; preds = %188
  %191 = call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  call void @llvm.trap()
  unreachable

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39: ; preds = %188
  %192 = getelementptr i64, ptr %166, i64 %.0112126
  %193 = getelementptr i64, ptr %167, i64 %.0112126
  call void @rl_m_insert__DictTint64_tTint64_tT_int64_t_int64_t_r_bool(ptr nonnull %21, ptr nonnull %22, ptr %192, ptr %193)
  %.not3 = icmp eq i64 %184, %163
  br i1 %.not3, label %.lr.ph129, label %183

194:                                              ; preds = %.lr.ph129, %rl_m_drop__String.exit54
  %.0110128 = phi i64 [ 0, %.lr.ph129 ], [ %199, %rl_m_drop__String.exit54 ]
  %195 = icmp slt i64 %.0110128, %163
  br i1 %195, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit40, label %196

196:                                              ; preds = %194
  %197 = call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  call void @llvm.trap()
  unreachable

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit40: ; preds = %194
  %198 = getelementptr i64, ptr %168, i64 %.0110128
  %199 = add nuw nsw i64 %.0110128, 1
  store ptr @str_21, ptr %20, align 8
  call void @rl_s__strlit_r_String(ptr nonnull %19, ptr nonnull %20)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %18, ptr %198)
  call void @rl_m_add__String_String_r_String(ptr nonnull %17, ptr nonnull %19, ptr nonnull %18)
  store ptr @str_22, ptr %16, align 8
  call void @rl_s__strlit_r_String(ptr nonnull %15, ptr nonnull %16)
  call void @rl_m_add__String_String_r_String(ptr nonnull %14, ptr nonnull %17, ptr nonnull %15)
  call void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr nonnull %13, ptr nonnull %41, ptr %198)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %12, ptr nonnull %13)
  call void @rl_m_add__String_String_r_String(ptr nonnull %11, ptr nonnull %14, ptr nonnull %12)
  call void @rl_print_string__String(ptr nonnull %11)
  %200 = load i64, ptr %169, align 8
  %.not3.i.i41 = icmp eq i64 %200, 0
  br i1 %.not3.i.i41, label %rl_m_drop__String.exit42, label %201

201:                                              ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit40
  %202 = load ptr, ptr %19, align 8
  call void @free(ptr %202)
  br label %rl_m_drop__String.exit42

rl_m_drop__String.exit42:                         ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit40, %201
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %170, i8 0, i64 16, i1 false)
  %203 = load i64, ptr %171, align 8
  %.not3.i.i43 = icmp eq i64 %203, 0
  br i1 %.not3.i.i43, label %rl_m_drop__String.exit44, label %204

204:                                              ; preds = %rl_m_drop__String.exit42
  %205 = load ptr, ptr %18, align 8
  call void @free(ptr %205)
  br label %rl_m_drop__String.exit44

rl_m_drop__String.exit44:                         ; preds = %rl_m_drop__String.exit42, %204
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %172, i8 0, i64 16, i1 false)
  %206 = load i64, ptr %173, align 8
  %.not3.i.i45 = icmp eq i64 %206, 0
  br i1 %.not3.i.i45, label %rl_m_drop__String.exit46, label %207

207:                                              ; preds = %rl_m_drop__String.exit44
  %208 = load ptr, ptr %17, align 8
  call void @free(ptr %208)
  br label %rl_m_drop__String.exit46

rl_m_drop__String.exit46:                         ; preds = %rl_m_drop__String.exit44, %207
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %174, i8 0, i64 16, i1 false)
  %209 = load i64, ptr %175, align 8
  %.not3.i.i47 = icmp eq i64 %209, 0
  br i1 %.not3.i.i47, label %rl_m_drop__String.exit48, label %210

210:                                              ; preds = %rl_m_drop__String.exit46
  %211 = load ptr, ptr %15, align 8
  call void @free(ptr %211)
  br label %rl_m_drop__String.exit48

rl_m_drop__String.exit48:                         ; preds = %rl_m_drop__String.exit46, %210
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %176, i8 0, i64 16, i1 false)
  %212 = load i64, ptr %177, align 8
  %.not3.i.i49 = icmp eq i64 %212, 0
  br i1 %.not3.i.i49, label %rl_m_drop__String.exit50, label %213

213:                                              ; preds = %rl_m_drop__String.exit48
  %214 = load ptr, ptr %14, align 8
  call void @free(ptr %214)
  br label %rl_m_drop__String.exit50

rl_m_drop__String.exit50:                         ; preds = %rl_m_drop__String.exit48, %213
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %178, i8 0, i64 16, i1 false)
  %215 = load i64, ptr %179, align 8
  %.not3.i.i51 = icmp eq i64 %215, 0
  br i1 %.not3.i.i51, label %rl_m_drop__String.exit52, label %216

216:                                              ; preds = %rl_m_drop__String.exit50
  %217 = load ptr, ptr %12, align 8
  call void @free(ptr %217)
  br label %rl_m_drop__String.exit52

rl_m_drop__String.exit52:                         ; preds = %rl_m_drop__String.exit50, %216
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %180, i8 0, i64 16, i1 false)
  %218 = load i64, ptr %181, align 8
  %.not3.i.i53 = icmp eq i64 %218, 0
  br i1 %.not3.i.i53, label %rl_m_drop__String.exit54, label %219

219:                                              ; preds = %rl_m_drop__String.exit52
  %220 = load ptr, ptr %11, align 8
  call void @free(ptr %220)
  br label %rl_m_drop__String.exit54

rl_m_drop__String.exit54:                         ; preds = %rl_m_drop__String.exit52, %219
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %182, i8 0, i64 16, i1 false)
  %.not4 = icmp eq i64 %199, %163
  br i1 %.not4, label %._crit_edge, label %194

._crit_edge:                                      ; preds = %rl_m_drop__String.exit54, %.split
  %221 = getelementptr inbounds i8, ptr %23, i64 8
  %222 = load i64, ptr %221, align 8
  %.not5130 = icmp eq i64 %222, 0
  br i1 %.not5130, label %._crit_edge134, label %.lr.ph133

.lr.ph133:                                        ; preds = %._crit_edge
  %223 = load ptr, ptr %23, align 8
  %224 = getelementptr inbounds i8, ptr %9, i64 16
  %225 = getelementptr inbounds i8, ptr %9, i64 8
  %226 = getelementptr inbounds i8, ptr %8, i64 16
  %227 = getelementptr inbounds i8, ptr %8, i64 8
  %228 = getelementptr inbounds i8, ptr %7, i64 16
  %229 = getelementptr inbounds i8, ptr %7, i64 8
  %230 = getelementptr inbounds i8, ptr %5, i64 16
  %231 = getelementptr inbounds i8, ptr %5, i64 8
  %232 = getelementptr inbounds i8, ptr %4, i64 16
  %233 = getelementptr inbounds i8, ptr %4, i64 8
  %234 = getelementptr inbounds i8, ptr %2, i64 16
  %235 = getelementptr inbounds i8, ptr %2, i64 8
  %236 = getelementptr inbounds i8, ptr %1, i64 16
  %237 = getelementptr inbounds i8, ptr %1, i64 8
  br label %238

238:                                              ; preds = %.lr.ph133, %rl_m_drop__String.exit69
  %.0131 = phi i64 [ 0, %.lr.ph133 ], [ %243, %rl_m_drop__String.exit69 ]
  %239 = icmp slt i64 %.0131, %222
  br i1 %239, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit55, label %240

240:                                              ; preds = %238
  %241 = call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  call void @llvm.trap()
  unreachable

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit55: ; preds = %238
  %242 = getelementptr i64, ptr %223, i64 %.0131
  %243 = add nuw nsw i64 %.0131, 1
  store ptr @str_23, ptr %10, align 8
  call void @rl_s__strlit_r_String(ptr nonnull %9, ptr nonnull %10)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %8, ptr %242)
  call void @rl_m_add__String_String_r_String(ptr nonnull %7, ptr nonnull %9, ptr nonnull %8)
  store ptr @str_22, ptr %6, align 8
  call void @rl_s__strlit_r_String(ptr nonnull %5, ptr nonnull %6)
  call void @rl_m_add__String_String_r_String(ptr nonnull %4, ptr nonnull %7, ptr nonnull %5)
  call void @rl_m_get__DictTint64_tTint64_tT_int64_t_r_int64_t(ptr nonnull %3, ptr nonnull %22, ptr %242)
  call void @rl_to_string__int64_t_r_String(ptr nonnull %2, ptr nonnull %3)
  call void @rl_m_add__String_String_r_String(ptr nonnull %1, ptr nonnull %4, ptr nonnull %2)
  call void @rl_print_string__String(ptr nonnull %1)
  %244 = load i64, ptr %224, align 8
  %.not3.i.i56 = icmp eq i64 %244, 0
  br i1 %.not3.i.i56, label %rl_m_drop__String.exit57, label %245

245:                                              ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit55
  %246 = load ptr, ptr %9, align 8
  call void @free(ptr %246)
  br label %rl_m_drop__String.exit57

rl_m_drop__String.exit57:                         ; preds = %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit55, %245
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %225, i8 0, i64 16, i1 false)
  %247 = load i64, ptr %226, align 8
  %.not3.i.i58 = icmp eq i64 %247, 0
  br i1 %.not3.i.i58, label %rl_m_drop__String.exit59, label %248

248:                                              ; preds = %rl_m_drop__String.exit57
  %249 = load ptr, ptr %8, align 8
  call void @free(ptr %249)
  br label %rl_m_drop__String.exit59

rl_m_drop__String.exit59:                         ; preds = %rl_m_drop__String.exit57, %248
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %227, i8 0, i64 16, i1 false)
  %250 = load i64, ptr %228, align 8
  %.not3.i.i60 = icmp eq i64 %250, 0
  br i1 %.not3.i.i60, label %rl_m_drop__String.exit61, label %251

251:                                              ; preds = %rl_m_drop__String.exit59
  %252 = load ptr, ptr %7, align 8
  call void @free(ptr %252)
  br label %rl_m_drop__String.exit61

rl_m_drop__String.exit61:                         ; preds = %rl_m_drop__String.exit59, %251
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %229, i8 0, i64 16, i1 false)
  %253 = load i64, ptr %230, align 8
  %.not3.i.i62 = icmp eq i64 %253, 0
  br i1 %.not3.i.i62, label %rl_m_drop__String.exit63, label %254

254:                                              ; preds = %rl_m_drop__String.exit61
  %255 = load ptr, ptr %5, align 8
  call void @free(ptr %255)
  br label %rl_m_drop__String.exit63

rl_m_drop__String.exit63:                         ; preds = %rl_m_drop__String.exit61, %254
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %231, i8 0, i64 16, i1 false)
  %256 = load i64, ptr %232, align 8
  %.not3.i.i64 = icmp eq i64 %256, 0
  br i1 %.not3.i.i64, label %rl_m_drop__String.exit65, label %257

257:                                              ; preds = %rl_m_drop__String.exit63
  %258 = load ptr, ptr %4, align 8
  call void @free(ptr %258)
  br label %rl_m_drop__String.exit65

rl_m_drop__String.exit65:                         ; preds = %rl_m_drop__String.exit63, %257
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %233, i8 0, i64 16, i1 false)
  %259 = load i64, ptr %234, align 8
  %.not3.i.i66 = icmp eq i64 %259, 0
  br i1 %.not3.i.i66, label %rl_m_drop__String.exit67, label %260

260:                                              ; preds = %rl_m_drop__String.exit65
  %261 = load ptr, ptr %2, align 8
  call void @free(ptr %261)
  br label %rl_m_drop__String.exit67

rl_m_drop__String.exit67:                         ; preds = %rl_m_drop__String.exit65, %260
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %235, i8 0, i64 16, i1 false)
  %262 = load i64, ptr %236, align 8
  %.not3.i.i68 = icmp eq i64 %262, 0
  br i1 %.not3.i.i68, label %rl_m_drop__String.exit69, label %263

263:                                              ; preds = %rl_m_drop__String.exit67
  %264 = load ptr, ptr %1, align 8
  call void @free(ptr %264)
  br label %rl_m_drop__String.exit69

rl_m_drop__String.exit69:                         ; preds = %rl_m_drop__String.exit67, %263
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %237, i8 0, i64 16, i1 false)
  %.not5 = icmp eq i64 %243, %222
  br i1 %.not5, label %._crit_edge134, label %238

._crit_edge134:                                   ; preds = %rl_m_drop__String.exit69, %._crit_edge
  %265 = load ptr, ptr %41, align 8
  call void @free(ptr %265)
  %266 = load i64, ptr %155, align 8
  %.not.i72 = icmp eq i64 %266, 0
  br i1 %.not.i72, label %rl_m_drop__DictTint64_tTint64_tT.exit74, label %267

267:                                              ; preds = %._crit_edge134
  %268 = load ptr, ptr %22, align 8
  call void @free(ptr %268)
  br label %rl_m_drop__DictTint64_tTint64_tT.exit74

rl_m_drop__DictTint64_tTint64_tT.exit74:          ; preds = %._crit_edge134, %267
  %269 = getelementptr inbounds i8, ptr %24, i64 16
  %270 = load i64, ptr %269, align 8
  %.not3.i = icmp eq i64 %270, 0
  br i1 %.not3.i, label %rl_m_drop__VectorTint64_tT.exit, label %271

271:                                              ; preds = %rl_m_drop__DictTint64_tTint64_tT.exit74
  %272 = load ptr, ptr %24, align 8
  call void @free(ptr %272)
  br label %rl_m_drop__VectorTint64_tT.exit

rl_m_drop__VectorTint64_tT.exit:                  ; preds = %rl_m_drop__DictTint64_tTint64_tT.exit74, %271
  %273 = getelementptr inbounds i8, ptr %23, i64 16
  %274 = load i64, ptr %273, align 8
  %.not3.i75 = icmp eq i64 %274, 0
  br i1 %.not3.i75, label %rl_m_drop__DictTint64_tTint64_tT.exit78, label %275

275:                                              ; preds = %rl_m_drop__VectorTint64_tT.exit
  %276 = load ptr, ptr %23, align 8
  call void @free(ptr %276)
  br label %rl_m_drop__DictTint64_tTint64_tT.exit78

rl_m_drop__DictTint64_tTint64_tT.exit78:          ; preds = %275, %rl_m_drop__VectorTint64_tT.exit
  store i64 0, ptr %0, align 1
  ret void
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #10

; Function Attrs: cold noreturn nounwind memory(inaccessiblemem: write)
declare void @llvm.trap() #11

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smax.i64(i64, i64) #12

; Function Attrs: nofree nounwind willreturn allockind("alloc,zeroed") allocsize(0,1) memory(inaccessiblemem: readwrite)
declare noalias noundef ptr @calloc(i64 noundef, i64 noundef) local_unnamed_addr #13

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #14

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #14

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.vector.reduce.add.v2i64(<2 x i64>) #12

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
attributes #11 = { cold noreturn nounwind memory(inaccessiblemem: write) }
attributes #12 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #13 = { nofree nounwind willreturn allockind("alloc,zeroed") allocsize(0,1) memory(inaccessiblemem: readwrite) "alloc-family"="malloc" }
attributes #14 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }

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
!12 = distinct !{!12, !2, !3}
!13 = distinct !{!13, !2}
!14 = distinct !{!14, !2, !3}
!15 = distinct !{!15, !2}
!16 = distinct !{!16, !2, !3}
!17 = distinct !{!17, !2, !3}
!18 = distinct !{!18, !2}
!19 = distinct !{!19, !2, !3}
!20 = distinct !{!20, !2}
!21 = distinct !{!21, !2, !3}
!22 = distinct !{!22, !2, !3}
!23 = distinct !{!23, !2}
!24 = distinct !{!24, !2, !3}
!25 = distinct !{!25, !2, !3}
!26 = distinct !{!26, !2}
!27 = distinct !{!27, !2, !3}
!28 = distinct !{!28, !2, !3}
!29 = distinct !{!29, !2}
!30 = distinct !{!30, !2, !3}
!31 = distinct !{!31, !2, !3}
!32 = distinct !{!32, !2}
!33 = distinct !{!33, !2, !3}
!34 = distinct !{!34, !2, !3}
!35 = distinct !{!35, !2}
!36 = distinct !{!36, !2, !3}
!37 = distinct !{!37, !2, !3}
!38 = distinct !{!38, !2}
!39 = distinct !{!39, !2, !3}
!40 = distinct !{!40, !2, !3}
!41 = distinct !{!41, !2}
!42 = distinct !{!42, !2, !3}
!43 = distinct !{!43, !2, !3}
!44 = distinct !{!44, !2}
!45 = distinct !{!45, !2, !3}
!46 = distinct !{!46, !2, !3}
!47 = distinct !{!47, !2}
!48 = distinct !{!48, !2, !3}
!49 = distinct !{!49, !2, !3}
!50 = distinct !{!50, !2}
!51 = distinct !{!51, !2, !3}
!52 = distinct !{!52, !2, !3}
!53 = distinct !{!53, !2}
!54 = distinct !{!54, !2, !3}
!55 = distinct !{!55, !2, !3}
!56 = distinct !{!56, !2}
!57 = distinct !{!57, !2, !3}
!58 = distinct !{!58, !2, !3}
!59 = distinct !{!59, !2}
!60 = distinct !{!60, !2, !3}
!61 = distinct !{!61, !2, !3}
!62 = distinct !{!62, !2}
!63 = distinct !{!63, !2, !3}
!64 = distinct !{!64, !2, !3}
!65 = distinct !{!65, !2}
!66 = distinct !{!66, !67}
!67 = !{!"llvm.loop.peeled.count", i32 1}
!68 = distinct !{!68, !2, !3}
!69 = distinct !{!69, !2, !3}
!70 = distinct !{!70, !2}
!71 = distinct !{!71, !2, !3}
!72 = distinct !{!72, !2, !3}
!73 = distinct !{!73, !2}
!74 = distinct !{!74, !2, !3}
!75 = distinct !{!75, !2, !3}
!76 = distinct !{!76, !2}
!77 = distinct !{!77, !2, !3}
!78 = distinct !{!78, !2, !3}
!79 = distinct !{!79, !2}
!80 = distinct !{!80, !2, !3}
!81 = distinct !{!81, !2, !3}
!82 = distinct !{!82, !2}
!83 = distinct !{!83, !2, !3}
!84 = distinct !{!84, !2, !3}
!85 = distinct !{!85, !2}
!86 = distinct !{!86, !2, !3}
!87 = distinct !{!87, !2, !3}
!88 = distinct !{!88, !2}
!89 = distinct !{!89, !2, !3}
!90 = distinct !{!90, !2, !3}
!91 = distinct !{!91, !2}
!92 = distinct !{!92, !2, !3}
!93 = distinct !{!93, !2, !3}
!94 = distinct !{!94, !2}
!95 = distinct !{!95, !2, !3}
!96 = distinct !{!96, !2, !3}
!97 = distinct !{!97, !2}
!98 = distinct !{!98, !2, !3}
!99 = distinct !{!99, !3, !2}
!100 = distinct !{!100, !2, !3}
!101 = distinct !{!101, !2, !3}
!102 = distinct !{!102, !2}
!103 = distinct !{!103, !2, !3}
!104 = distinct !{!104, !2, !3}
!105 = distinct !{!105, !2}
!106 = distinct !{!106, !2, !3}
!107 = distinct !{!107, !2, !3}
!108 = distinct !{!108, !2}
!109 = distinct !{!109, !2, !3}
!110 = distinct !{!110, !2, !3}
!111 = distinct !{!111, !2}
!112 = distinct !{!112, !2, !3}
!113 = distinct !{!113, !2, !3}
!114 = distinct !{!114, !2}
!115 = distinct !{!115, !2, !3}
!116 = distinct !{!116, !2, !3}
!117 = distinct !{!117, !2}
!118 = distinct !{!118, !119}
!119 = !{!"llvm.loop.unswitch.partial.disable"}
