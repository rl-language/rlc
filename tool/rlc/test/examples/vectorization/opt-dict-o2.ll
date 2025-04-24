; ModuleID = 'dict.ll'
source_filename = "dict.rl"
target datalayout = "S128-e-p272:64:64:64:64-p271:32:32:32:32-i64:64-f80:128-i128:128-i1:8-p0:64:64:64:64-i16:16-i8:8-i32:32-f128:128-f16:16-p270:32:32:32:32-f64:64"
target triple = "x86_64-unknown-linux-gnu"

%Entry = type { i8, i64, i64, i64 }
%Vector.1 = type { ptr, i64, i64 }
%String = type { %Vector }
%Vector = type { ptr, i64, i64 }
%Dict = type { ptr, i64, i64, double }

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
  %2 = alloca %Vector.1, align 8
  %3 = alloca %Vector.1, align 8
  %4 = getelementptr inbounds i8, ptr %3, i64 8
  store i64 0, ptr %4, align 8
  %5 = getelementptr inbounds i8, ptr %3, i64 16
  store i64 4, ptr %5, align 8
  %calloc22 = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  store ptr %calloc22, ptr %3, align 8
  %6 = getelementptr i8, ptr %1, i64 8
  %7 = load i64, ptr %6, align 8
  %8 = icmp sgt i64 %7, 0
  br i1 %8, label %.lr.ph, label %rl_m_init__VectorTint64_tT.exit._crit_edge

.lr.ph:                                           ; preds = %rl_m_init__VectorTint64_tT.exit.preheader, %rl_m_init__VectorTint64_tT.exit
  %9 = phi i64 [ %42, %rl_m_init__VectorTint64_tT.exit ], [ %7, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.017 = phi i64 [ %.1, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.0516 = phi i64 [ %45, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %10 = phi i64 [ %44, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %11 = phi i64 [ %43, %rl_m_init__VectorTint64_tT.exit ], [ 4, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.pre2.i1415 = phi ptr [ %.pre2.i13, %rl_m_init__VectorTint64_tT.exit ], [ %calloc22, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.pre2.i141527 = ptrtoint ptr %.pre2.i1415 to i64
  %12 = load ptr, ptr %1, align 8
  %13 = getelementptr %Entry, ptr %12, i64 %.0516
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
  %22 = ptrtoint ptr %21 to i64
  %23 = trunc i64 %17 to i63
  %24 = icmp sgt i63 %23, 0
  br i1 %24, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %19
  tail call void @llvm.memset.p0.i64(ptr align 8 %21, i8 0, i64 %20, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %19
  %25 = icmp sgt i64 %10, 0
  br i1 %25, label %.lr.ph15.i.i.preheader, label %.preheader.i.i

.lr.ph15.i.i.preheader:                           ; preds = %.preheader12.i.i
  %min.iters.check = icmp ult i64 %10, 4
  %26 = sub i64 %22, %.pre2.i141527
  %diff.check = icmp ult i64 %26, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader29, label %vector.ph

.lr.ph15.i.i.preheader29:                         ; preds = %middle.block, %.lr.ph15.i.i.preheader
  %.114.i.i.ph = phi i64 [ 0, %.lr.ph15.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph15.i.i

vector.ph:                                        ; preds = %.lr.ph15.i.i.preheader
  %n.vec = and i64 %10, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %27 = getelementptr i64, ptr %21, i64 %index
  %28 = getelementptr i64, ptr %.pre2.i1415, i64 %index
  %29 = getelementptr i8, ptr %28, i64 16
  %wide.load = load <2 x i64>, ptr %28, align 8
  %wide.load28 = load <2 x i64>, ptr %29, align 8
  %30 = getelementptr i8, ptr %27, i64 16
  store <2 x i64> %wide.load, ptr %27, align 8
  store <2 x i64> %wide.load28, ptr %30, align 8
  %index.next = add nuw i64 %index, 4
  %31 = icmp eq i64 %index.next, %n.vec
  br i1 %31, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %10, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph15.i.i.preheader29

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %.preheader12.i.i
  tail call void @free(ptr %.pre2.i1415)
  %32 = shl i64 %17, 1
  br label %rl_m_append__VectorTint64_tT_int64_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader29, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %36, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader29 ]
  %33 = getelementptr i64, ptr %21, i64 %.114.i.i
  %34 = getelementptr i64, ptr %.pre2.i1415, i64 %.114.i.i
  %35 = load i64, ptr %34, align 8
  store i64 %35, ptr %33, align 8
  %36 = add nuw nsw i64 %.114.i.i, 1
  %37 = icmp slt i64 %36, %10
  br i1 %37, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !4

rl_m_append__VectorTint64_tT_int64_t.exit:        ; preds = %15, %.preheader.i.i
  %.pre2.i12 = phi ptr [ %21, %.preheader.i.i ], [ %.pre2.i1415, %15 ]
  %38 = phi i64 [ %32, %.preheader.i.i ], [ %11, %15 ]
  %39 = getelementptr i64, ptr %.pre2.i12, i64 %10
  %40 = load i64, ptr %16, align 8
  store i64 %40, ptr %39, align 8
  %41 = add i64 %.017, 1
  %.pre = load i64, ptr %6, align 8
  br label %rl_m_init__VectorTint64_tT.exit

rl_m_init__VectorTint64_tT.exit:                  ; preds = %.lr.ph, %rl_m_append__VectorTint64_tT_int64_t.exit
  %42 = phi i64 [ %.pre, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %9, %.lr.ph ]
  %.pre2.i13 = phi ptr [ %.pre2.i12, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %.pre2.i1415, %.lr.ph ]
  %43 = phi i64 [ %38, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %11, %.lr.ph ]
  %44 = phi i64 [ %17, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %10, %.lr.ph ]
  %.1 = phi i64 [ %41, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %.017, %.lr.ph ]
  %45 = add i64 %.0516, 1
  %46 = icmp slt i64 %.1, %42
  br i1 %46, label %.lr.ph, label %rl_m_init__VectorTint64_tT.exit._crit_edge

rl_m_init__VectorTint64_tT.exit._crit_edge:       ; preds = %rl_m_init__VectorTint64_tT.exit, %rl_m_init__VectorTint64_tT.exit.preheader
  %47 = phi ptr [ %calloc22, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %.pre2.i13, %rl_m_init__VectorTint64_tT.exit ]
  %.lcssa10 = phi i64 [ 4, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %43, %rl_m_init__VectorTint64_tT.exit ]
  %.lcssa = phi i64 [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %44, %rl_m_init__VectorTint64_tT.exit ]
  store i64 %.lcssa, ptr %4, align 8
  store i64 %.lcssa10, ptr %5, align 8
  store ptr %47, ptr %3, align 8
  %48 = getelementptr inbounds i8, ptr %2, i64 8
  store i64 0, ptr %48, align 8
  %49 = getelementptr inbounds i8, ptr %2, i64 16
  store i64 4, ptr %49, align 8
  %calloc = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  store ptr %calloc, ptr %2, align 8
  call void @rl_m_assign__VectorTint64_tT_VectorTint64_tT(ptr nonnull %2, ptr nonnull %3)
  %.not3.i = icmp eq i64 %.lcssa10, 0
  br i1 %.not3.i, label %rl_m_drop__VectorTint64_tT.exit, label %50

50:                                               ; preds = %rl_m_init__VectorTint64_tT.exit._crit_edge
  tail call void @free(ptr %47)
  br label %rl_m_drop__VectorTint64_tT.exit

rl_m_drop__VectorTint64_tT.exit:                  ; preds = %rl_m_init__VectorTint64_tT.exit._crit_edge, %50
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
  %calloc22 = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  store ptr %calloc22, ptr %3, align 8
  %6 = getelementptr i8, ptr %1, i64 8
  %7 = load i64, ptr %6, align 8
  %8 = icmp sgt i64 %7, 0
  br i1 %8, label %.lr.ph, label %rl_m_init__VectorTint64_tT.exit._crit_edge

.lr.ph:                                           ; preds = %rl_m_init__VectorTint64_tT.exit.preheader, %rl_m_init__VectorTint64_tT.exit
  %9 = phi i64 [ %42, %rl_m_init__VectorTint64_tT.exit ], [ %7, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.017 = phi i64 [ %.1, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.0516 = phi i64 [ %45, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %10 = phi i64 [ %44, %rl_m_init__VectorTint64_tT.exit ], [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %11 = phi i64 [ %43, %rl_m_init__VectorTint64_tT.exit ], [ 4, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.pre2.i1415 = phi ptr [ %.pre2.i13, %rl_m_init__VectorTint64_tT.exit ], [ %calloc22, %rl_m_init__VectorTint64_tT.exit.preheader ]
  %.pre2.i141527 = ptrtoint ptr %.pre2.i1415 to i64
  %12 = load ptr, ptr %1, align 8
  %13 = getelementptr %Entry, ptr %12, i64 %.0516
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
  %22 = ptrtoint ptr %21 to i64
  %23 = trunc i64 %17 to i63
  %24 = icmp sgt i63 %23, 0
  br i1 %24, label %.lr.ph.preheader.i.i, label %.preheader12.i.i

.lr.ph.preheader.i.i:                             ; preds = %19
  tail call void @llvm.memset.p0.i64(ptr align 8 %21, i8 0, i64 %20, i1 false)
  br label %.preheader12.i.i

.preheader12.i.i:                                 ; preds = %.lr.ph.preheader.i.i, %19
  %25 = icmp sgt i64 %10, 0
  br i1 %25, label %.lr.ph15.i.i.preheader, label %.preheader.i.i

.lr.ph15.i.i.preheader:                           ; preds = %.preheader12.i.i
  %min.iters.check = icmp ult i64 %10, 4
  %26 = sub i64 %22, %.pre2.i141527
  %diff.check = icmp ult i64 %26, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.preheader29, label %vector.ph

.lr.ph15.i.i.preheader29:                         ; preds = %middle.block, %.lr.ph15.i.i.preheader
  %.114.i.i.ph = phi i64 [ 0, %.lr.ph15.i.i.preheader ], [ %n.vec, %middle.block ]
  br label %.lr.ph15.i.i

vector.ph:                                        ; preds = %.lr.ph15.i.i.preheader
  %n.vec = and i64 %10, 9223372036854775804
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %27 = getelementptr i64, ptr %21, i64 %index
  %28 = getelementptr i64, ptr %.pre2.i1415, i64 %index
  %29 = getelementptr i8, ptr %28, i64 16
  %wide.load = load <2 x i64>, ptr %28, align 8
  %wide.load28 = load <2 x i64>, ptr %29, align 8
  %30 = getelementptr i8, ptr %27, i64 16
  store <2 x i64> %wide.load, ptr %27, align 8
  store <2 x i64> %wide.load28, ptr %30, align 8
  %index.next = add nuw i64 %index, 4
  %31 = icmp eq i64 %index.next, %n.vec
  br i1 %31, label %middle.block, label %vector.body, !llvm.loop !5

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %10, %n.vec
  br i1 %cmp.n, label %.preheader.i.i, label %.lr.ph15.i.i.preheader29

.preheader.i.i:                                   ; preds = %.lr.ph15.i.i, %middle.block, %.preheader12.i.i
  tail call void @free(ptr %.pre2.i1415)
  %32 = shl i64 %17, 1
  br label %rl_m_append__VectorTint64_tT_int64_t.exit

.lr.ph15.i.i:                                     ; preds = %.lr.ph15.i.i.preheader29, %.lr.ph15.i.i
  %.114.i.i = phi i64 [ %36, %.lr.ph15.i.i ], [ %.114.i.i.ph, %.lr.ph15.i.i.preheader29 ]
  %33 = getelementptr i64, ptr %21, i64 %.114.i.i
  %34 = getelementptr i64, ptr %.pre2.i1415, i64 %.114.i.i
  %35 = load i64, ptr %34, align 8
  store i64 %35, ptr %33, align 8
  %36 = add nuw nsw i64 %.114.i.i, 1
  %37 = icmp slt i64 %36, %10
  br i1 %37, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !6

rl_m_append__VectorTint64_tT_int64_t.exit:        ; preds = %15, %.preheader.i.i
  %.pre2.i12 = phi ptr [ %21, %.preheader.i.i ], [ %.pre2.i1415, %15 ]
  %38 = phi i64 [ %32, %.preheader.i.i ], [ %11, %15 ]
  %39 = getelementptr i64, ptr %.pre2.i12, i64 %10
  %40 = load i64, ptr %16, align 8
  store i64 %40, ptr %39, align 8
  %41 = add i64 %.017, 1
  %.pre = load i64, ptr %6, align 8
  br label %rl_m_init__VectorTint64_tT.exit

rl_m_init__VectorTint64_tT.exit:                  ; preds = %.lr.ph, %rl_m_append__VectorTint64_tT_int64_t.exit
  %42 = phi i64 [ %.pre, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %9, %.lr.ph ]
  %.pre2.i13 = phi ptr [ %.pre2.i12, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %.pre2.i1415, %.lr.ph ]
  %43 = phi i64 [ %38, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %11, %.lr.ph ]
  %44 = phi i64 [ %17, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %10, %.lr.ph ]
  %.1 = phi i64 [ %41, %rl_m_append__VectorTint64_tT_int64_t.exit ], [ %.017, %.lr.ph ]
  %45 = add i64 %.0516, 1
  %46 = icmp slt i64 %.1, %42
  br i1 %46, label %.lr.ph, label %rl_m_init__VectorTint64_tT.exit._crit_edge

rl_m_init__VectorTint64_tT.exit._crit_edge:       ; preds = %rl_m_init__VectorTint64_tT.exit, %rl_m_init__VectorTint64_tT.exit.preheader
  %47 = phi ptr [ %calloc22, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %.pre2.i13, %rl_m_init__VectorTint64_tT.exit ]
  %.lcssa10 = phi i64 [ 4, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %43, %rl_m_init__VectorTint64_tT.exit ]
  %.lcssa = phi i64 [ 0, %rl_m_init__VectorTint64_tT.exit.preheader ], [ %44, %rl_m_init__VectorTint64_tT.exit ]
  store i64 %.lcssa, ptr %4, align 8
  store i64 %.lcssa10, ptr %5, align 8
  store ptr %47, ptr %3, align 8
  %48 = getelementptr inbounds i8, ptr %2, i64 8
  store i64 0, ptr %48, align 8
  %49 = getelementptr inbounds i8, ptr %2, i64 16
  store i64 4, ptr %49, align 8
  %calloc = tail call dereferenceable_or_null(32) ptr @calloc(i64 1, i64 32)
  store ptr %calloc, ptr %2, align 8
  call void @rl_m_assign__VectorTint64_tT_VectorTint64_tT(ptr nonnull %2, ptr nonnull %3)
  %.not3.i = icmp eq i64 %.lcssa10, 0
  br i1 %.not3.i, label %rl_m_drop__VectorTint64_tT.exit, label %50

50:                                               ; preds = %rl_m_init__VectorTint64_tT.exit._crit_edge
  tail call void @free(ptr %47)
  br label %rl_m_drop__VectorTint64_tT.exit

rl_m_drop__VectorTint64_tT.exit:                  ; preds = %rl_m_init__VectorTint64_tT.exit._crit_edge, %50
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
  %.not39 = icmp sgt i64 %14, 0
  br i1 %.not39, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %3
  %15 = load ptr, ptr %1, align 8
  br label %16

16:                                               ; preds = %.lr.ph, %33
  %.pn = phi i64 [ %12, %.lr.ph ], [ %34, %33 ]
  %.01941 = phi i64 [ 0, %.lr.ph ], [ %17, %33 ]
  %.042 = srem i64 %.pn, %14
  %17 = add nuw nsw i64 %.01941, 1
  %18 = getelementptr %Entry, ptr %15, i64 %.042
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
  %.not32 = icmp eq i64 %27, %4
  br i1 %.not32, label %35, label %.thread

.thread:                                          ; preds = %21, %25
  %28 = add i64 %.042, %14
  %29 = srem i64 %23, %14
  %30 = sub i64 %28, %29
  %31 = srem i64 %30, %14
  %32 = icmp slt i64 %31, %.01941
  br i1 %32, label %common.ret, label %33

33:                                               ; preds = %.thread
  %34 = add i64 %.042, 1
  %.not = icmp slt i64 %17, %14
  br i1 %.not, label %16, label %._crit_edge

35:                                               ; preds = %25
  %36 = getelementptr i8, ptr %1, i64 8
  %37 = load i64, ptr %36, align 8
  %38 = add i64 %37, -1
  store i64 %38, ptr %36, align 8
  %39 = add i64 %.042, 1
  %40 = srem i64 %39, %14
  %41 = getelementptr %Entry, ptr %15, i64 %40
  %42 = load i8, ptr %41, align 1
  %43 = icmp eq i8 %42, 0
  br i1 %43, label %._crit_edge47, label %.lr.ph46

.lr.ph46:                                         ; preds = %35, %53
  %44 = phi i64 [ %59, %53 ], [ %14, %35 ]
  %.pn49 = phi ptr [ %62, %53 ], [ %41, %35 ]
  %45 = phi i8 [ %63, %53 ], [ %42, %35 ]
  %46 = phi ptr [ %61, %53 ], [ %15, %35 ]
  %.02244 = phi i64 [ %60, %53 ], [ %40, %35 ]
  %.02343 = phi i64 [ %.02244, %53 ], [ %.042, %35 ]
  %.in52 = getelementptr i8, ptr %.pn49, i64 8
  %47 = load i64, ptr %.in52, align 8
  %48 = add i64 %44, %.02244
  %49 = srem i64 %47, %44
  %50 = sub i64 %48, %49
  %51 = srem i64 %50, %44
  %52 = icmp eq i64 %51, 0
  br i1 %52, label %65, label %53

53:                                               ; preds = %.lr.ph46
  %.in50 = getelementptr i8, ptr %.pn49, i64 16
  %54 = getelementptr %Entry, ptr %46, i64 %.02343
  %55 = getelementptr i8, ptr %54, i64 8
  %56 = getelementptr i8, ptr %54, i64 16
  %57 = load <2 x i64>, ptr %.in50, align 8
  store i8 %45, ptr %54, align 1
  store i64 %47, ptr %55, align 8
  store <2 x i64> %57, ptr %56, align 8
  %58 = add i64 %.02244, 1
  %59 = load i64, ptr %13, align 8
  %60 = srem i64 %58, %59
  %61 = load ptr, ptr %1, align 8
  %62 = getelementptr %Entry, ptr %61, i64 %60
  %63 = load i8, ptr %62, align 1
  %64 = icmp eq i8 %63, 0
  br i1 %64, label %._crit_edge47, label %.lr.ph46

65:                                               ; preds = %.lr.ph46
  %66 = getelementptr %Entry, ptr %46, i64 %.02343
  br label %common.ret.sink.split

._crit_edge47:                                    ; preds = %53, %35
  %.023.lcssa = phi i64 [ %.042, %35 ], [ %.02244, %53 ]
  %.lcssa = phi ptr [ %15, %35 ], [ %61, %53 ]
  %67 = getelementptr %Entry, ptr %.lcssa, i64 %.023.lcssa
  br label %common.ret.sink.split

common.ret.sink.split:                            ; preds = %._crit_edge47, %65
  %.sink = phi ptr [ %66, %65 ], [ %67, %._crit_edge47 ]
  store i8 0, ptr %.sink, align 1
  br label %common.ret

common.ret:                                       ; preds = %.thread, %16, %common.ret.sink.split
  %storemerge = phi i8 [ 1, %common.ret.sink.split ], [ 0, %16 ], [ 0, %.thread ]
  store i8 %storemerge, ptr %0, align 1
  ret void

._crit_edge:                                      ; preds = %33, %3
  %68 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_2)
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
  %.not22 = icmp sgt i64 %18, 0
  br i1 %.not22, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %7
  %19 = load ptr, ptr %1, align 8
  br label %21

._crit_edge:                                      ; preds = %38, %7
  %20 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_3)
  tail call void @llvm.trap()
  unreachable

21:                                               ; preds = %.lr.ph, %38
  %.pn = phi i64 [ %16, %.lr.ph ], [ %39, %38 ]
  %.01124 = phi i64 [ 0, %.lr.ph ], [ %22, %38 ]
  %.025 = srem i64 %.pn, %18
  %22 = add nuw nsw i64 %.01124, 1
  %23 = getelementptr %Entry, ptr %19, i64 %.025
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
  %.not20 = icmp eq i64 %32, %8
  br i1 %.not20, label %common.ret, label %.thread

.thread:                                          ; preds = %26, %30
  %33 = add i64 %.025, %18
  %34 = srem i64 %28, %18
  %35 = sub i64 %33, %34
  %36 = srem i64 %35, %18
  %37 = icmp slt i64 %36, %.01124
  br i1 %37, label %common.ret, label %38

38:                                               ; preds = %.thread
  %39 = add i64 %.025, 1
  %.not = icmp slt i64 %22, %18
  br i1 %.not, label %21, label %._crit_edge

common.ret:                                       ; preds = %.thread, %21, %30, %3
  %storemerge = phi i8 [ 0, %3 ], [ 1, %30 ], [ 0, %.thread ], [ 0, %21 ]
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
  %.not21 = icmp sgt i64 %20, 0
  br i1 %.not21, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %9
  %21 = load ptr, ptr %1, align 8
  br label %23

._crit_edge:                                      ; preds = %42, %9
  %22 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_5)
  tail call void @llvm.trap()
  unreachable

23:                                               ; preds = %.lr.ph, %42
  %.pn = phi i64 [ %18, %.lr.ph ], [ %43, %42 ]
  %.0923 = phi i64 [ 0, %.lr.ph ], [ %24, %42 ]
  %.024 = srem i64 %.pn, %20
  %24 = add nuw nsw i64 %.0923, 1
  %25 = getelementptr %Entry, ptr %21, i64 %.024
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
  %.not17 = icmp eq i64 %34, %10
  br i1 %.not17, label %44, label %.thread

.thread:                                          ; preds = %28, %32
  %35 = add i64 %.024, %20
  %36 = srem i64 %30, %20
  %37 = sub i64 %35, %36
  %38 = srem i64 %37, %20
  %39 = icmp slt i64 %38, %.0923
  br i1 %39, label %40, label %42

40:                                               ; preds = %.thread
  %41 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_6)
  tail call void @llvm.trap()
  unreachable

42:                                               ; preds = %.thread
  %43 = add i64 %.024, 1
  %.not = icmp slt i64 %24, %20
  br i1 %.not, label %23, label %._crit_edge

44:                                               ; preds = %32
  %45 = getelementptr i8, ptr %25, i64 24
  %46 = load i64, ptr %45, align 8
  store i64 %46, ptr %0, align 1
  ret void

47:                                               ; preds = %23
  %48 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_7)
  tail call void @llvm.trap()
  unreachable
}

; Function Attrs: nounwind
define internal fastcc void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr nocapture %0, ptr nocapture readonly %1, ptr nocapture readonly %2, ptr nocapture readonly %3) unnamed_addr #5 {
  %5 = getelementptr i8, ptr %0, i64 16
  %6 = load i64, ptr %5, align 8
  %.not58 = icmp sgt i64 %6, 0
  br i1 %.not58, label %.lr.ph.preheader, label %._crit_edge

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
  %.064 = phi i64 [ %53, %49 ], [ %17, %.lr.ph.preheader ]
  %.02663 = phi i64 [ %51, %49 ], [ 0, %.lr.ph.preheader ]
  %.02762 = phi i64 [ %19, %49 ], [ 0, %.lr.ph.preheader ]
  %.02861 = phi i64 [ %.129, %49 ], [ %7, %.lr.ph.preheader ]
  %.03060 = phi i64 [ %.131, %49 ], [ %16, %.lr.ph.preheader ]
  %.04159 = phi i64 [ %.142, %49 ], [ %8, %.lr.ph.preheader ]
  %19 = add nuw nsw i64 %.02762, 1
  %20 = load ptr, ptr %1, align 8
  %21 = getelementptr %Entry, ptr %20, i64 %.064
  %22 = load i8, ptr %21, align 1
  %23 = icmp eq i8 %22, 0
  %24 = getelementptr i8, ptr %21, i64 8
  br i1 %23, label %65, label %25

25:                                               ; preds = %.lr.ph
  %26 = load i64, ptr %24, align 8
  %27 = icmp eq i64 %26, %.03060
  br i1 %27, label %28, label %.thread

28:                                               ; preds = %25
  %29 = getelementptr i8, ptr %21, i64 16
  %30 = load i64, ptr %29, align 8
  %.not45 = icmp eq i64 %30, %.04159
  br i1 %.not45, label %54, label %.thread

.thread:                                          ; preds = %25, %28
  %31 = add i64 %18, %.064
  %32 = srem i64 %26, %18
  %33 = sub i64 %31, %32
  %34 = srem i64 %33, %18
  %35 = icmp slt i64 %34, %.02663
  br i1 %35, label %36, label %49

36:                                               ; preds = %.thread
  %37 = getelementptr i8, ptr %21, i64 16
  %38 = load i64, ptr %37, align 8
  %39 = getelementptr i8, ptr %21, i64 24
  %40 = load i64, ptr %39, align 8
  store i64 %.03060, ptr %24, align 8
  store i64 %.04159, ptr %37, align 8
  store i64 %.02861, ptr %39, align 8
  %41 = load ptr, ptr %1, align 8
  %42 = getelementptr %Entry, ptr %41, i64 %.064
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
  %.142 = phi i64 [ %38, %36 ], [ %.04159, %.thread ]
  %.131 = phi i64 [ %26, %36 ], [ %.03060, %.thread ]
  %.129 = phi i64 [ %40, %36 ], [ %.02861, %.thread ]
  %.1 = phi i64 [ %34, %36 ], [ %.02663, %.thread ]
  %51 = add nsw i64 %.1, 1
  %52 = add i64 %.064, 1
  %53 = srem i64 %52, %50
  %.not = icmp slt i64 %19, %50
  br i1 %.not, label %.lr.ph, label %._crit_edge

common.ret:                                       ; preds = %65, %54
  ret void

54:                                               ; preds = %28
  %55 = getelementptr i8, ptr %21, i64 16
  %56 = getelementptr i8, ptr %21, i64 24
  store i64 %.02861, ptr %56, align 8
  %57 = load ptr, ptr %1, align 8
  %58 = getelementptr %Entry, ptr %57, i64 %.064
  store i8 %22, ptr %58, align 1
  %59 = getelementptr i8, ptr %58, i64 8
  %60 = load i64, ptr %24, align 8
  store i64 %60, ptr %59, align 8
  %61 = getelementptr i8, ptr %58, i64 16
  %62 = load i64, ptr %55, align 8
  store i64 %62, ptr %61, align 8
  %63 = getelementptr i8, ptr %58, i64 24
  %64 = load i64, ptr %56, align 8
  store i64 %64, ptr %63, align 8
  br label %common.ret

65:                                               ; preds = %.lr.ph
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %24, i8 0, i64 24, i1 false)
  %66 = load ptr, ptr %1, align 8
  %67 = getelementptr %Entry, ptr %66, i64 %.064
  %68 = getelementptr i8, ptr %67, i64 8
  %69 = getelementptr i8, ptr %67, i64 16
  %70 = getelementptr i8, ptr %67, i64 24
  store i8 1, ptr %67, align 1
  store i64 %.03060, ptr %68, align 8
  store i64 %.04159, ptr %69, align 8
  store i64 %.02861, ptr %70, align 8
  %71 = getelementptr i8, ptr %0, i64 8
  %72 = load i64, ptr %71, align 8
  %73 = add i64 %72, 1
  store i64 %73, ptr %71, align 8
  br label %common.ret

._crit_edge:                                      ; preds = %49, %4
  %74 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_8)
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
  br i1 %24, label %.lr.ph.i, label %.preheader17.i

.preheader17.i:                                   ; preds = %.lr.ph.i, %rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i
  %25 = icmp sgt i64 %10, 0
  br i1 %25, label %.lr.ph20.i, label %rl_m__grow__DictTint64_tTint64_tT.exit

.lr.ph.i:                                         ; preds = %rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i, %.lr.ph.i
  %.018.i = phi i64 [ %28, %.lr.ph.i ], [ 0, %rl_m__next_power_of_2__DictTint64_tTint64_tT_int64_t_r_int64_t.exit.i ]
  %26 = load ptr, ptr %1, align 8
  %27 = getelementptr %Entry, ptr %26, i64 %.018.i
  store i8 0, ptr %27, align 1
  %28 = add nuw nsw i64 %.018.i, 1
  %29 = load i64, ptr %9, align 8
  %30 = icmp slt i64 %28, %29
  br i1 %30, label %.lr.ph.i, label %.preheader17.i

.lr.ph20.i:                                       ; preds = %.preheader17.i, %36
  %.119.i = phi i64 [ %37, %36 ], [ 0, %.preheader17.i ]
  %31 = getelementptr %Entry, ptr %17, i64 %.119.i
  %32 = load i8, ptr %31, align 1
  %.not.i = icmp eq i8 %32, 0
  br i1 %.not.i, label %36, label %33

33:                                               ; preds = %.lr.ph20.i
  %34 = getelementptr i8, ptr %31, i64 16
  %35 = getelementptr i8, ptr %31, i64 24
  tail call fastcc void @rl_m__insert__DictTint64_tTint64_tT_EntryTint64_tTint64_tTPtr_int64_t_int64_t(ptr nonnull %1, ptr nonnull %1, ptr %34, ptr %35)
  br label %36

36:                                               ; preds = %33, %.lr.ph20.i
  %37 = add nuw nsw i64 %.119.i, 1
  %38 = icmp slt i64 %37, %10
  br i1 %38, label %.lr.ph20.i, label %rl_m__grow__DictTint64_tTint64_tT.exit

rl_m__grow__DictTint64_tTint64_tT.exit:           ; preds = %36, %.preheader17.i
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
  br i1 %20, label %middle.block, label %vector.body, !llvm.loop !7

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
  br i1 %23, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !8

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
  br i1 %28, label %.lr.ph15.i, label %.preheader.i, !llvm.loop !9

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
  br i1 %21, label %middle.block, label %vector.body, !llvm.loop !10

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
  br i1 %27, label %.lr.ph15.i, label %.preheader.i, !llvm.loop !11

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
  br i1 %34, label %middle.block, label %vector.body, !llvm.loop !12

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
  br i1 %37, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !13

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
  br i1 %42, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !14

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
  br i1 %35, label %middle.block, label %vector.body, !llvm.loop !15

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
  br i1 %41, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !16

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

.lr.ph:                                           ; preds = %rl_m_init__String.exit, %268
  %14 = phi i64 [ %270, %268 ], [ %11, %rl_m_init__String.exit ]
  %.0149 = phi i64 [ %269, %268 ], [ 0, %rl_m_init__String.exit ]
  %.0140148 = phi i64 [ %.1141, %268 ], [ 0, %rl_m_init__String.exit ]
  %15 = icmp sgt i64 %.0149, -1
  br i1 %15, label %18, label %16

16:                                               ; preds = %.lr.ph
  %17 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

18:                                               ; preds = %.lr.ph
  %19 = icmp slt i64 %.0149, %14
  br i1 %19, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %20

20:                                               ; preds = %18
  %21 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %28 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
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
  br i1 %40, label %.lr.ph.preheader.i.i.i12, label %iter.check

.lr.ph.preheader.i.i.i12:                         ; preds = %36
  tail call void @llvm.memset.p0.i64(ptr align 1 %38, i8 0, i64 %37, i1 false)
  br label %iter.check

iter.check:                                       ; preds = %36, %.lr.ph.preheader.i.i.i12
  %smax = tail call i64 @llvm.smax.i64(i64 %25, i64 1)
  %min.iters.check = icmp slt i64 %25, 8
  %41 = sub i64 %39, %30
  %diff.check = icmp ult i64 %41, 32
  %or.cond = or i1 %min.iters.check, %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.i10.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check152 = icmp slt i64 %25, 32
  br i1 %min.iters.check152, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %smax, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %42 = getelementptr i8, ptr %38, i64 %index
  %43 = getelementptr i8, ptr %29, i64 %index
  %44 = getelementptr i8, ptr %43, i64 16
  %wide.load = load <16 x i8>, ptr %43, align 1
  %wide.load153 = load <16 x i8>, ptr %44, align 1
  %45 = getelementptr i8, ptr %42, i64 16
  store <16 x i8> %wide.load, ptr %42, align 1
  store <16 x i8> %wide.load153, ptr %45, align 1
  %index.next = add nuw i64 %index, 32
  %46 = icmp eq i64 %index.next, %n.vec
  br i1 %46, label %middle.block, label %vector.body, !llvm.loop !17

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %smax, %n.vec
  br i1 %cmp.n, label %.preheader.i.i.i8, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %smax, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.i10.preheader, label %vec.epilog.ph

.lr.ph15.i.i.i10.preheader:                       ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.i11.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec155, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i.i10

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec155 = and i64 %smax, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index156 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next158, %vec.epilog.vector.body ]
  %47 = getelementptr i8, ptr %38, i64 %index156
  %48 = getelementptr i8, ptr %29, i64 %index156
  %wide.load157 = load <8 x i8>, ptr %48, align 1
  store <8 x i8> %wide.load157, ptr %47, align 1
  %index.next158 = add nuw i64 %index156, 8
  %49 = icmp eq i64 %index.next158, %n.vec155
  br i1 %49, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !18

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n159 = icmp eq i64 %smax, %n.vec155
  br i1 %cmp.n159, label %.preheader.i.i.i8, label %.lr.ph15.i.i.i10.preheader

.preheader.i.i.i8:                                ; preds = %.lr.ph15.i.i.i10, %vec.epilog.middle.block, %middle.block
  tail call void @free(ptr nonnull %29)
  store i64 %37, ptr %8, align 8
  store ptr %38, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit

.lr.ph15.i.i.i10:                                 ; preds = %.lr.ph15.i.i.i10.preheader, %.lr.ph15.i.i.i10
  %.114.i.i.i11 = phi i64 [ %53, %.lr.ph15.i.i.i10 ], [ %.114.i.i.i11.ph, %.lr.ph15.i.i.i10.preheader ]
  %50 = getelementptr i8, ptr %38, i64 %.114.i.i.i11
  %51 = getelementptr i8, ptr %29, i64 %.114.i.i.i11
  %52 = load i8, ptr %51, align 1
  store i8 %52, ptr %50, align 1
  %53 = add nuw nsw i64 %.114.i.i.i11, 1
  %54 = icmp slt i64 %53, %25
  br i1 %54, label %.lr.ph15.i.i.i10, label %.preheader.i.i.i8, !llvm.loop !19

rl_m_append__String_int8_t.exit:                  ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i, %.preheader.i.i.i8
  %55 = phi ptr [ %38, %.preheader.i.i.i8 ], [ %29, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i ]
  %56 = getelementptr i8, ptr %55, i64 %25
  store i8 0, ptr %56, align 1
  store i64 %33, ptr %7, align 8
  br label %268

rl_m_get__String_int64_t_r_int8_tRef.exit15:      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %57 = load i64, ptr %7, align 8
  %58 = icmp sgt i64 %57, 0
  br i1 %58, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16, label %59

59:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit15
  %60 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16:   ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit15
  %61 = load ptr, ptr %6, align 8
  %62 = ptrtoint ptr %61 to i64
  %63 = getelementptr i8, ptr %61, i64 %57
  %64 = getelementptr i8, ptr %63, i64 -1
  store i8 44, ptr %64, align 1
  %65 = add nuw i64 %57, 1
  %66 = load i64, ptr %8, align 8
  %67 = icmp sgt i64 %66, %65
  br i1 %67, label %rl_m_append__String_int8_t.exit26, label %68

68:                                               ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16
  %69 = shl i64 %65, 1
  %70 = tail call ptr @malloc(i64 %69)
  %71 = ptrtoint ptr %70 to i64
  %72 = icmp sgt i64 %69, 0
  br i1 %72, label %.lr.ph.preheader.i.i.i23, label %iter.check332

.lr.ph.preheader.i.i.i23:                         ; preds = %68
  tail call void @llvm.memset.p0.i64(ptr align 1 %70, i8 0, i64 %69, i1 false)
  br label %iter.check332

iter.check332:                                    ; preds = %68, %.lr.ph.preheader.i.i.i23
  %smax328 = tail call i64 @llvm.smax.i64(i64 %57, i64 1)
  %min.iters.check330 = icmp slt i64 %57, 8
  %73 = sub i64 %71, %62
  %diff.check327 = icmp ult i64 %73, 32
  %or.cond359 = or i1 %min.iters.check330, %diff.check327
  br i1 %or.cond359, label %.lr.ph15.i.i.i21.preheader, label %vector.main.loop.iter.check334

vector.main.loop.iter.check334:                   ; preds = %iter.check332
  %min.iters.check333 = icmp slt i64 %57, 32
  br i1 %min.iters.check333, label %vec.epilog.ph346, label %vector.ph335

vector.ph335:                                     ; preds = %vector.main.loop.iter.check334
  %n.vec337 = and i64 %smax328, 9223372036854775776
  br label %vector.body338

vector.body338:                                   ; preds = %vector.body338, %vector.ph335
  %index339 = phi i64 [ 0, %vector.ph335 ], [ %index.next342, %vector.body338 ]
  %74 = getelementptr i8, ptr %70, i64 %index339
  %75 = getelementptr i8, ptr %61, i64 %index339
  %76 = getelementptr i8, ptr %75, i64 16
  %wide.load340 = load <16 x i8>, ptr %75, align 1
  %wide.load341 = load <16 x i8>, ptr %76, align 1
  %77 = getelementptr i8, ptr %74, i64 16
  store <16 x i8> %wide.load340, ptr %74, align 1
  store <16 x i8> %wide.load341, ptr %77, align 1
  %index.next342 = add nuw i64 %index339, 32
  %78 = icmp eq i64 %index.next342, %n.vec337
  br i1 %78, label %middle.block329, label %vector.body338, !llvm.loop !20

middle.block329:                                  ; preds = %vector.body338
  %cmp.n343 = icmp eq i64 %smax328, %n.vec337
  br i1 %cmp.n343, label %.preheader.i.i.i19, label %vec.epilog.iter.check347

vec.epilog.iter.check347:                         ; preds = %middle.block329
  %n.vec.remaining348 = and i64 %smax328, 24
  %min.epilog.iters.check349 = icmp eq i64 %n.vec.remaining348, 0
  br i1 %min.epilog.iters.check349, label %.lr.ph15.i.i.i21.preheader, label %vec.epilog.ph346

.lr.ph15.i.i.i21.preheader:                       ; preds = %vec.epilog.middle.block344, %iter.check332, %vec.epilog.iter.check347
  %.114.i.i.i22.ph = phi i64 [ 0, %iter.check332 ], [ %n.vec337, %vec.epilog.iter.check347 ], [ %n.vec352, %vec.epilog.middle.block344 ]
  br label %.lr.ph15.i.i.i21

vec.epilog.ph346:                                 ; preds = %vector.main.loop.iter.check334, %vec.epilog.iter.check347
  %vec.epilog.resume.val350 = phi i64 [ %n.vec337, %vec.epilog.iter.check347 ], [ 0, %vector.main.loop.iter.check334 ]
  %n.vec352 = and i64 %smax328, 9223372036854775800
  br label %vec.epilog.vector.body354

vec.epilog.vector.body354:                        ; preds = %vec.epilog.vector.body354, %vec.epilog.ph346
  %index355 = phi i64 [ %vec.epilog.resume.val350, %vec.epilog.ph346 ], [ %index.next357, %vec.epilog.vector.body354 ]
  %79 = getelementptr i8, ptr %70, i64 %index355
  %80 = getelementptr i8, ptr %61, i64 %index355
  %wide.load356 = load <8 x i8>, ptr %80, align 1
  store <8 x i8> %wide.load356, ptr %79, align 1
  %index.next357 = add nuw i64 %index355, 8
  %81 = icmp eq i64 %index.next357, %n.vec352
  br i1 %81, label %vec.epilog.middle.block344, label %vec.epilog.vector.body354, !llvm.loop !21

vec.epilog.middle.block344:                       ; preds = %vec.epilog.vector.body354
  %cmp.n358 = icmp eq i64 %smax328, %n.vec352
  br i1 %cmp.n358, label %.preheader.i.i.i19, label %.lr.ph15.i.i.i21.preheader

.preheader.i.i.i19:                               ; preds = %.lr.ph15.i.i.i21, %vec.epilog.middle.block344, %middle.block329
  tail call void @free(ptr nonnull %61)
  store i64 %69, ptr %8, align 8
  store ptr %70, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit26

.lr.ph15.i.i.i21:                                 ; preds = %.lr.ph15.i.i.i21.preheader, %.lr.ph15.i.i.i21
  %.114.i.i.i22 = phi i64 [ %85, %.lr.ph15.i.i.i21 ], [ %.114.i.i.i22.ph, %.lr.ph15.i.i.i21.preheader ]
  %82 = getelementptr i8, ptr %70, i64 %.114.i.i.i22
  %83 = getelementptr i8, ptr %61, i64 %.114.i.i.i22
  %84 = load i8, ptr %83, align 1
  store i8 %84, ptr %82, align 1
  %85 = add nuw nsw i64 %.114.i.i.i22, 1
  %86 = icmp slt i64 %85, %57
  br i1 %86, label %.lr.ph15.i.i.i21, label %.preheader.i.i.i19, !llvm.loop !22

rl_m_append__String_int8_t.exit26:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16, %.preheader.i.i.i19
  %87 = phi i64 [ %69, %.preheader.i.i.i19 ], [ %66, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16 ]
  %.pre2.i.i36 = phi ptr [ %70, %.preheader.i.i.i19 ], [ %61, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i16 ]
  %.pre2.i.i36293 = ptrtoint ptr %.pre2.i.i36 to i64
  %88 = getelementptr i8, ptr %.pre2.i.i36, i64 %57
  store i8 0, ptr %88, align 1
  store i64 %65, ptr %7, align 8
  %.not = icmp eq i64 %57, 9223372036854775807
  br i1 %.not, label %89, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27

89:                                               ; preds = %rl_m_append__String_int8_t.exit26
  %90 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27:   ; preds = %rl_m_append__String_int8_t.exit26
  %91 = getelementptr i8, ptr %.pre2.i.i36, i64 %65
  %92 = getelementptr i8, ptr %91, i64 -1
  store i8 10, ptr %92, align 1
  %93 = add nuw i64 %57, 2
  %94 = icmp sgt i64 %87, %93
  br i1 %94, label %rl_m_append__String_int8_t.exit37, label %95

95:                                               ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27
  %96 = shl i64 %93, 1
  %97 = tail call ptr @malloc(i64 %96)
  %98 = ptrtoint ptr %97 to i64
  %99 = icmp sgt i64 %96, 0
  br i1 %99, label %.lr.ph.preheader.i.i.i34, label %iter.check299

.lr.ph.preheader.i.i.i34:                         ; preds = %95
  tail call void @llvm.memset.p0.i64(ptr align 1 %97, i8 0, i64 %96, i1 false)
  br label %iter.check299

iter.check299:                                    ; preds = %95, %.lr.ph.preheader.i.i.i34
  %smax295 = tail call i64 @llvm.smax.i64(i64 %65, i64 1)
  %min.iters.check297 = icmp slt i64 %65, 8
  %100 = sub i64 %98, %.pre2.i.i36293
  %diff.check294 = icmp ult i64 %100, 32
  %or.cond360 = or i1 %min.iters.check297, %diff.check294
  br i1 %or.cond360, label %.lr.ph15.i.i.i32.preheader, label %vector.main.loop.iter.check301

vector.main.loop.iter.check301:                   ; preds = %iter.check299
  %min.iters.check300 = icmp slt i64 %65, 32
  br i1 %min.iters.check300, label %vec.epilog.ph313, label %vector.ph302

vector.ph302:                                     ; preds = %vector.main.loop.iter.check301
  %n.vec304 = and i64 %smax295, 9223372036854775776
  br label %vector.body305

vector.body305:                                   ; preds = %vector.body305, %vector.ph302
  %index306 = phi i64 [ 0, %vector.ph302 ], [ %index.next309, %vector.body305 ]
  %101 = getelementptr i8, ptr %97, i64 %index306
  %102 = getelementptr i8, ptr %.pre2.i.i36, i64 %index306
  %103 = getelementptr i8, ptr %102, i64 16
  %wide.load307 = load <16 x i8>, ptr %102, align 1
  %wide.load308 = load <16 x i8>, ptr %103, align 1
  %104 = getelementptr i8, ptr %101, i64 16
  store <16 x i8> %wide.load307, ptr %101, align 1
  store <16 x i8> %wide.load308, ptr %104, align 1
  %index.next309 = add nuw i64 %index306, 32
  %105 = icmp eq i64 %index.next309, %n.vec304
  br i1 %105, label %middle.block296, label %vector.body305, !llvm.loop !23

middle.block296:                                  ; preds = %vector.body305
  %cmp.n310 = icmp eq i64 %smax295, %n.vec304
  br i1 %cmp.n310, label %.preheader.i.i.i30, label %vec.epilog.iter.check314

vec.epilog.iter.check314:                         ; preds = %middle.block296
  %n.vec.remaining315 = and i64 %smax295, 24
  %min.epilog.iters.check316 = icmp eq i64 %n.vec.remaining315, 0
  br i1 %min.epilog.iters.check316, label %.lr.ph15.i.i.i32.preheader, label %vec.epilog.ph313

.lr.ph15.i.i.i32.preheader:                       ; preds = %vec.epilog.middle.block311, %iter.check299, %vec.epilog.iter.check314
  %.114.i.i.i33.ph = phi i64 [ 0, %iter.check299 ], [ %n.vec304, %vec.epilog.iter.check314 ], [ %n.vec319, %vec.epilog.middle.block311 ]
  br label %.lr.ph15.i.i.i32

vec.epilog.ph313:                                 ; preds = %vector.main.loop.iter.check301, %vec.epilog.iter.check314
  %vec.epilog.resume.val317 = phi i64 [ %n.vec304, %vec.epilog.iter.check314 ], [ 0, %vector.main.loop.iter.check301 ]
  %n.vec319 = and i64 %smax295, 9223372036854775800
  br label %vec.epilog.vector.body321

vec.epilog.vector.body321:                        ; preds = %vec.epilog.vector.body321, %vec.epilog.ph313
  %index322 = phi i64 [ %vec.epilog.resume.val317, %vec.epilog.ph313 ], [ %index.next324, %vec.epilog.vector.body321 ]
  %106 = getelementptr i8, ptr %97, i64 %index322
  %107 = getelementptr i8, ptr %.pre2.i.i36, i64 %index322
  %wide.load323 = load <8 x i8>, ptr %107, align 1
  store <8 x i8> %wide.load323, ptr %106, align 1
  %index.next324 = add nuw i64 %index322, 8
  %108 = icmp eq i64 %index.next324, %n.vec319
  br i1 %108, label %vec.epilog.middle.block311, label %vec.epilog.vector.body321, !llvm.loop !24

vec.epilog.middle.block311:                       ; preds = %vec.epilog.vector.body321
  %cmp.n325 = icmp eq i64 %smax295, %n.vec319
  br i1 %cmp.n325, label %.preheader.i.i.i30, label %.lr.ph15.i.i.i32.preheader

.preheader.i.i.i30:                               ; preds = %.lr.ph15.i.i.i32, %vec.epilog.middle.block311, %middle.block296
  tail call void @free(ptr nonnull %.pre2.i.i36)
  store i64 %96, ptr %8, align 8
  store ptr %97, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit37

.lr.ph15.i.i.i32:                                 ; preds = %.lr.ph15.i.i.i32.preheader, %.lr.ph15.i.i.i32
  %.114.i.i.i33 = phi i64 [ %112, %.lr.ph15.i.i.i32 ], [ %.114.i.i.i33.ph, %.lr.ph15.i.i.i32.preheader ]
  %109 = getelementptr i8, ptr %97, i64 %.114.i.i.i33
  %110 = getelementptr i8, ptr %.pre2.i.i36, i64 %.114.i.i.i33
  %111 = load i8, ptr %110, align 1
  store i8 %111, ptr %109, align 1
  %112 = add nuw nsw i64 %.114.i.i.i33, 1
  %113 = icmp slt i64 %112, %65
  br i1 %113, label %.lr.ph15.i.i.i32, label %.preheader.i.i.i30, !llvm.loop !25

rl_m_append__String_int8_t.exit37:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27, %.preheader.i.i.i30
  %114 = phi ptr [ %97, %.preheader.i.i.i30 ], [ %.pre2.i.i36, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i27 ]
  %115 = getelementptr i8, ptr %114, i64 %65
  store i8 0, ptr %115, align 1
  %116 = load i64, ptr %7, align 8
  %117 = add i64 %116, 1
  store i64 %117, ptr %7, align 8
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %4)
  %.not2.i = icmp eq i64 %.0140148, 0
  br i1 %.not2.i, label %rl__indent_string__String_int64_t.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %rl_m_append__String_int8_t.exit37, %.lr.ph.i
  %.03.i = phi i64 [ %118, %.lr.ph.i ], [ 0, %rl_m_append__String_int8_t.exit37 ]
  store ptr @str_13, ptr %4, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %6, ptr nonnull %4)
  %118 = add nuw i64 %.03.i, 1
  %.not.i = icmp eq i64 %118, %.0140148
  br i1 %.not.i, label %rl__indent_string__String_int64_t.exit, label %.lr.ph.i

rl__indent_string__String_int64_t.exit:           ; preds = %.lr.ph.i, %rl_m_append__String_int8_t.exit37
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %4)
  %119 = add nuw i64 %.0149, 1
  %120 = icmp sgt i64 %119, -1
  br i1 %120, label %123, label %121

121:                                              ; preds = %rl__indent_string__String_int64_t.exit
  %122 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

123:                                              ; preds = %rl__indent_string__String_int64_t.exit
  %124 = load i64, ptr %10, align 8
  %125 = icmp slt i64 %119, %124
  br i1 %125, label %rl_m_get__String_int64_t_r_int8_tRef.exit38, label %126

126:                                              ; preds = %123
  %127 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit38:      ; preds = %123
  %128 = load ptr, ptr %1, align 8
  %129 = getelementptr i8, ptr %128, i64 %119
  %130 = load i8, ptr %129, align 1
  %131 = icmp eq i8 %130, 32
  %spec.select = select i1 %131, i64 %119, i64 %.0149
  br label %268

rl_is_close_paren__int8_t_r_bool.exit.thread:     ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %132 = load i64, ptr %7, align 8
  %133 = icmp sgt i64 %132, 0
  br i1 %133, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39, label %134

134:                                              ; preds = %rl_is_close_paren__int8_t_r_bool.exit.thread
  %135 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39:   ; preds = %rl_is_close_paren__int8_t_r_bool.exit.thread
  %136 = load ptr, ptr %6, align 8
  %137 = ptrtoint ptr %136 to i64
  %138 = getelementptr i8, ptr %136, i64 %132
  %139 = getelementptr i8, ptr %138, i64 -1
  store i8 10, ptr %139, align 1
  %140 = add nuw i64 %132, 1
  %141 = load i64, ptr %8, align 8
  %142 = icmp sgt i64 %141, %140
  br i1 %142, label %rl_m_append__String_int8_t.exit49, label %143

143:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39
  %144 = shl i64 %140, 1
  %145 = tail call ptr @malloc(i64 %144)
  %146 = ptrtoint ptr %145 to i64
  %147 = icmp sgt i64 %144, 0
  br i1 %147, label %.lr.ph.preheader.i.i.i46, label %iter.check265

.lr.ph.preheader.i.i.i46:                         ; preds = %143
  tail call void @llvm.memset.p0.i64(ptr align 1 %145, i8 0, i64 %144, i1 false)
  br label %iter.check265

iter.check265:                                    ; preds = %143, %.lr.ph.preheader.i.i.i46
  %smax261 = tail call i64 @llvm.smax.i64(i64 %132, i64 1)
  %min.iters.check263 = icmp slt i64 %132, 8
  %148 = sub i64 %146, %137
  %diff.check260 = icmp ult i64 %148, 32
  %or.cond361 = or i1 %min.iters.check263, %diff.check260
  br i1 %or.cond361, label %.lr.ph15.i.i.i44.preheader, label %vector.main.loop.iter.check267

vector.main.loop.iter.check267:                   ; preds = %iter.check265
  %min.iters.check266 = icmp slt i64 %132, 32
  br i1 %min.iters.check266, label %vec.epilog.ph279, label %vector.ph268

vector.ph268:                                     ; preds = %vector.main.loop.iter.check267
  %n.vec270 = and i64 %smax261, 9223372036854775776
  br label %vector.body271

vector.body271:                                   ; preds = %vector.body271, %vector.ph268
  %index272 = phi i64 [ 0, %vector.ph268 ], [ %index.next275, %vector.body271 ]
  %149 = getelementptr i8, ptr %145, i64 %index272
  %150 = getelementptr i8, ptr %136, i64 %index272
  %151 = getelementptr i8, ptr %150, i64 16
  %wide.load273 = load <16 x i8>, ptr %150, align 1
  %wide.load274 = load <16 x i8>, ptr %151, align 1
  %152 = getelementptr i8, ptr %149, i64 16
  store <16 x i8> %wide.load273, ptr %149, align 1
  store <16 x i8> %wide.load274, ptr %152, align 1
  %index.next275 = add nuw i64 %index272, 32
  %153 = icmp eq i64 %index.next275, %n.vec270
  br i1 %153, label %middle.block262, label %vector.body271, !llvm.loop !26

middle.block262:                                  ; preds = %vector.body271
  %cmp.n276 = icmp eq i64 %smax261, %n.vec270
  br i1 %cmp.n276, label %.preheader.i.i.i42, label %vec.epilog.iter.check280

vec.epilog.iter.check280:                         ; preds = %middle.block262
  %n.vec.remaining281 = and i64 %smax261, 24
  %min.epilog.iters.check282 = icmp eq i64 %n.vec.remaining281, 0
  br i1 %min.epilog.iters.check282, label %.lr.ph15.i.i.i44.preheader, label %vec.epilog.ph279

.lr.ph15.i.i.i44.preheader:                       ; preds = %vec.epilog.middle.block277, %iter.check265, %vec.epilog.iter.check280
  %.114.i.i.i45.ph = phi i64 [ 0, %iter.check265 ], [ %n.vec270, %vec.epilog.iter.check280 ], [ %n.vec285, %vec.epilog.middle.block277 ]
  br label %.lr.ph15.i.i.i44

vec.epilog.ph279:                                 ; preds = %vector.main.loop.iter.check267, %vec.epilog.iter.check280
  %vec.epilog.resume.val283 = phi i64 [ %n.vec270, %vec.epilog.iter.check280 ], [ 0, %vector.main.loop.iter.check267 ]
  %n.vec285 = and i64 %smax261, 9223372036854775800
  br label %vec.epilog.vector.body287

vec.epilog.vector.body287:                        ; preds = %vec.epilog.vector.body287, %vec.epilog.ph279
  %index288 = phi i64 [ %vec.epilog.resume.val283, %vec.epilog.ph279 ], [ %index.next290, %vec.epilog.vector.body287 ]
  %154 = getelementptr i8, ptr %145, i64 %index288
  %155 = getelementptr i8, ptr %136, i64 %index288
  %wide.load289 = load <8 x i8>, ptr %155, align 1
  store <8 x i8> %wide.load289, ptr %154, align 1
  %index.next290 = add nuw i64 %index288, 8
  %156 = icmp eq i64 %index.next290, %n.vec285
  br i1 %156, label %vec.epilog.middle.block277, label %vec.epilog.vector.body287, !llvm.loop !27

vec.epilog.middle.block277:                       ; preds = %vec.epilog.vector.body287
  %cmp.n291 = icmp eq i64 %smax261, %n.vec285
  br i1 %cmp.n291, label %.preheader.i.i.i42, label %.lr.ph15.i.i.i44.preheader

.preheader.i.i.i42:                               ; preds = %.lr.ph15.i.i.i44, %vec.epilog.middle.block277, %middle.block262
  tail call void @free(ptr nonnull %136)
  store i64 %144, ptr %8, align 8
  store ptr %145, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit49

.lr.ph15.i.i.i44:                                 ; preds = %.lr.ph15.i.i.i44.preheader, %.lr.ph15.i.i.i44
  %.114.i.i.i45 = phi i64 [ %160, %.lr.ph15.i.i.i44 ], [ %.114.i.i.i45.ph, %.lr.ph15.i.i.i44.preheader ]
  %157 = getelementptr i8, ptr %145, i64 %.114.i.i.i45
  %158 = getelementptr i8, ptr %136, i64 %.114.i.i.i45
  %159 = load i8, ptr %158, align 1
  store i8 %159, ptr %157, align 1
  %160 = add nuw nsw i64 %.114.i.i.i45, 1
  %161 = icmp slt i64 %160, %132
  br i1 %161, label %.lr.ph15.i.i.i44, label %.preheader.i.i.i42, !llvm.loop !28

rl_m_append__String_int8_t.exit49:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39, %.preheader.i.i.i42
  %162 = phi ptr [ %145, %.preheader.i.i.i42 ], [ %136, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i39 ]
  %163 = getelementptr i8, ptr %162, i64 %132
  store i8 0, ptr %163, align 1
  store i64 %140, ptr %7, align 8
  %164 = add i64 %.0140148, -1
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %3)
  %.not2.i50 = icmp eq i64 %164, 0
  br i1 %.not2.i50, label %.loopexit, label %.lr.ph.i51

.lr.ph.i51:                                       ; preds = %rl_m_append__String_int8_t.exit49, %.lr.ph.i51
  %.03.i52 = phi i64 [ %165, %.lr.ph.i51 ], [ 0, %rl_m_append__String_int8_t.exit49 ]
  store ptr @str_13, ptr %3, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %6, ptr nonnull %3)
  %165 = add nuw i64 %.03.i52, 1
  %.not.i53 = icmp eq i64 %165, %164
  br i1 %.not.i53, label %.loopexit, label %.lr.ph.i51

.loopexit:                                        ; preds = %.lr.ph.i51, %rl_m_append__String_int8_t.exit49
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %3)
  %166 = load i64, ptr %10, align 8
  %167 = icmp slt i64 %.0149, %166
  br i1 %167, label %rl_m_get__String_int64_t_r_int8_tRef.exit55, label %168

168:                                              ; preds = %.loopexit
  %169 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit55:      ; preds = %.loopexit
  %170 = load i64, ptr %7, align 8
  %171 = icmp sgt i64 %170, 0
  br i1 %171, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56, label %172

172:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit55
  %173 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56:   ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit55
  %174 = load ptr, ptr %1, align 8
  %175 = getelementptr i8, ptr %174, i64 %.0149
  %176 = load ptr, ptr %6, align 8
  %177 = ptrtoint ptr %176 to i64
  %178 = getelementptr i8, ptr %176, i64 %170
  %179 = getelementptr i8, ptr %178, i64 -1
  %180 = load i8, ptr %175, align 1
  store i8 %180, ptr %179, align 1
  %181 = add nuw i64 %170, 1
  %182 = load i64, ptr %8, align 8
  %183 = icmp sgt i64 %182, %181
  br i1 %183, label %rl_m_append__String_int8_t.exit66, label %184

184:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56
  %185 = shl i64 %181, 1
  %186 = tail call ptr @malloc(i64 %185)
  %187 = ptrtoint ptr %186 to i64
  %188 = icmp sgt i64 %185, 0
  br i1 %188, label %.lr.ph.preheader.i.i.i63, label %iter.check232

.lr.ph.preheader.i.i.i63:                         ; preds = %184
  tail call void @llvm.memset.p0.i64(ptr align 1 %186, i8 0, i64 %185, i1 false)
  br label %iter.check232

iter.check232:                                    ; preds = %184, %.lr.ph.preheader.i.i.i63
  %smax228 = tail call i64 @llvm.smax.i64(i64 %170, i64 1)
  %min.iters.check230 = icmp slt i64 %170, 8
  %189 = sub i64 %187, %177
  %diff.check227 = icmp ult i64 %189, 32
  %or.cond362 = or i1 %min.iters.check230, %diff.check227
  br i1 %or.cond362, label %.lr.ph15.i.i.i61.preheader, label %vector.main.loop.iter.check234

vector.main.loop.iter.check234:                   ; preds = %iter.check232
  %min.iters.check233 = icmp slt i64 %170, 32
  br i1 %min.iters.check233, label %vec.epilog.ph246, label %vector.ph235

vector.ph235:                                     ; preds = %vector.main.loop.iter.check234
  %n.vec237 = and i64 %smax228, 9223372036854775776
  br label %vector.body238

vector.body238:                                   ; preds = %vector.body238, %vector.ph235
  %index239 = phi i64 [ 0, %vector.ph235 ], [ %index.next242, %vector.body238 ]
  %190 = getelementptr i8, ptr %186, i64 %index239
  %191 = getelementptr i8, ptr %176, i64 %index239
  %192 = getelementptr i8, ptr %191, i64 16
  %wide.load240 = load <16 x i8>, ptr %191, align 1
  %wide.load241 = load <16 x i8>, ptr %192, align 1
  %193 = getelementptr i8, ptr %190, i64 16
  store <16 x i8> %wide.load240, ptr %190, align 1
  store <16 x i8> %wide.load241, ptr %193, align 1
  %index.next242 = add nuw i64 %index239, 32
  %194 = icmp eq i64 %index.next242, %n.vec237
  br i1 %194, label %middle.block229, label %vector.body238, !llvm.loop !29

middle.block229:                                  ; preds = %vector.body238
  %cmp.n243 = icmp eq i64 %smax228, %n.vec237
  br i1 %cmp.n243, label %.preheader.i.i.i59, label %vec.epilog.iter.check247

vec.epilog.iter.check247:                         ; preds = %middle.block229
  %n.vec.remaining248 = and i64 %smax228, 24
  %min.epilog.iters.check249 = icmp eq i64 %n.vec.remaining248, 0
  br i1 %min.epilog.iters.check249, label %.lr.ph15.i.i.i61.preheader, label %vec.epilog.ph246

.lr.ph15.i.i.i61.preheader:                       ; preds = %vec.epilog.middle.block244, %iter.check232, %vec.epilog.iter.check247
  %.114.i.i.i62.ph = phi i64 [ 0, %iter.check232 ], [ %n.vec237, %vec.epilog.iter.check247 ], [ %n.vec252, %vec.epilog.middle.block244 ]
  br label %.lr.ph15.i.i.i61

vec.epilog.ph246:                                 ; preds = %vector.main.loop.iter.check234, %vec.epilog.iter.check247
  %vec.epilog.resume.val250 = phi i64 [ %n.vec237, %vec.epilog.iter.check247 ], [ 0, %vector.main.loop.iter.check234 ]
  %n.vec252 = and i64 %smax228, 9223372036854775800
  br label %vec.epilog.vector.body254

vec.epilog.vector.body254:                        ; preds = %vec.epilog.vector.body254, %vec.epilog.ph246
  %index255 = phi i64 [ %vec.epilog.resume.val250, %vec.epilog.ph246 ], [ %index.next257, %vec.epilog.vector.body254 ]
  %195 = getelementptr i8, ptr %186, i64 %index255
  %196 = getelementptr i8, ptr %176, i64 %index255
  %wide.load256 = load <8 x i8>, ptr %196, align 1
  store <8 x i8> %wide.load256, ptr %195, align 1
  %index.next257 = add nuw i64 %index255, 8
  %197 = icmp eq i64 %index.next257, %n.vec252
  br i1 %197, label %vec.epilog.middle.block244, label %vec.epilog.vector.body254, !llvm.loop !30

vec.epilog.middle.block244:                       ; preds = %vec.epilog.vector.body254
  %cmp.n258 = icmp eq i64 %smax228, %n.vec252
  br i1 %cmp.n258, label %.preheader.i.i.i59, label %.lr.ph15.i.i.i61.preheader

.preheader.i.i.i59:                               ; preds = %.lr.ph15.i.i.i61, %vec.epilog.middle.block244, %middle.block229
  tail call void @free(ptr nonnull %176)
  store i64 %185, ptr %8, align 8
  store ptr %186, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit66

.lr.ph15.i.i.i61:                                 ; preds = %.lr.ph15.i.i.i61.preheader, %.lr.ph15.i.i.i61
  %.114.i.i.i62 = phi i64 [ %201, %.lr.ph15.i.i.i61 ], [ %.114.i.i.i62.ph, %.lr.ph15.i.i.i61.preheader ]
  %198 = getelementptr i8, ptr %186, i64 %.114.i.i.i62
  %199 = getelementptr i8, ptr %176, i64 %.114.i.i.i62
  %200 = load i8, ptr %199, align 1
  store i8 %200, ptr %198, align 1
  %201 = add nuw nsw i64 %.114.i.i.i62, 1
  %202 = icmp slt i64 %201, %170
  br i1 %202, label %.lr.ph15.i.i.i61, label %.preheader.i.i.i59, !llvm.loop !31

rl_m_append__String_int8_t.exit66:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56, %.preheader.i.i.i59
  %203 = phi ptr [ %186, %.preheader.i.i.i59 ], [ %176, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i56 ]
  %204 = getelementptr i8, ptr %203, i64 %170
  store i8 0, ptr %204, align 1
  store i64 %181, ptr %7, align 8
  br label %268

rl_m_get__String_int64_t_r_int8_tRef.exit67:      ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %205 = load i64, ptr %7, align 8
  %206 = icmp sgt i64 %205, 0
  br i1 %206, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68, label %207

207:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit67
  %208 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68:   ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit67
  %209 = load ptr, ptr %6, align 8
  %210 = ptrtoint ptr %209 to i64
  %211 = getelementptr i8, ptr %209, i64 %205
  %212 = getelementptr i8, ptr %211, i64 -1
  store i8 %24, ptr %212, align 1
  %213 = add nuw i64 %205, 1
  %214 = load i64, ptr %8, align 8
  %215 = icmp sgt i64 %214, %213
  br i1 %215, label %rl_m_append__String_int8_t.exit78, label %216

216:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68
  %217 = shl i64 %213, 1
  %218 = tail call ptr @malloc(i64 %217)
  %219 = ptrtoint ptr %218 to i64
  %220 = icmp sgt i64 %217, 0
  br i1 %220, label %.lr.ph.preheader.i.i.i75, label %iter.check199

.lr.ph.preheader.i.i.i75:                         ; preds = %216
  tail call void @llvm.memset.p0.i64(ptr align 1 %218, i8 0, i64 %217, i1 false)
  br label %iter.check199

iter.check199:                                    ; preds = %216, %.lr.ph.preheader.i.i.i75
  %smax195 = tail call i64 @llvm.smax.i64(i64 %205, i64 1)
  %min.iters.check197 = icmp slt i64 %205, 8
  %221 = sub i64 %219, %210
  %diff.check194 = icmp ult i64 %221, 32
  %or.cond363 = or i1 %min.iters.check197, %diff.check194
  br i1 %or.cond363, label %.lr.ph15.i.i.i73.preheader, label %vector.main.loop.iter.check201

vector.main.loop.iter.check201:                   ; preds = %iter.check199
  %min.iters.check200 = icmp slt i64 %205, 32
  br i1 %min.iters.check200, label %vec.epilog.ph213, label %vector.ph202

vector.ph202:                                     ; preds = %vector.main.loop.iter.check201
  %n.vec204 = and i64 %smax195, 9223372036854775776
  br label %vector.body205

vector.body205:                                   ; preds = %vector.body205, %vector.ph202
  %index206 = phi i64 [ 0, %vector.ph202 ], [ %index.next209, %vector.body205 ]
  %222 = getelementptr i8, ptr %218, i64 %index206
  %223 = getelementptr i8, ptr %209, i64 %index206
  %224 = getelementptr i8, ptr %223, i64 16
  %wide.load207 = load <16 x i8>, ptr %223, align 1
  %wide.load208 = load <16 x i8>, ptr %224, align 1
  %225 = getelementptr i8, ptr %222, i64 16
  store <16 x i8> %wide.load207, ptr %222, align 1
  store <16 x i8> %wide.load208, ptr %225, align 1
  %index.next209 = add nuw i64 %index206, 32
  %226 = icmp eq i64 %index.next209, %n.vec204
  br i1 %226, label %middle.block196, label %vector.body205, !llvm.loop !32

middle.block196:                                  ; preds = %vector.body205
  %cmp.n210 = icmp eq i64 %smax195, %n.vec204
  br i1 %cmp.n210, label %.preheader.i.i.i71, label %vec.epilog.iter.check214

vec.epilog.iter.check214:                         ; preds = %middle.block196
  %n.vec.remaining215 = and i64 %smax195, 24
  %min.epilog.iters.check216 = icmp eq i64 %n.vec.remaining215, 0
  br i1 %min.epilog.iters.check216, label %.lr.ph15.i.i.i73.preheader, label %vec.epilog.ph213

.lr.ph15.i.i.i73.preheader:                       ; preds = %vec.epilog.middle.block211, %iter.check199, %vec.epilog.iter.check214
  %.114.i.i.i74.ph = phi i64 [ 0, %iter.check199 ], [ %n.vec204, %vec.epilog.iter.check214 ], [ %n.vec219, %vec.epilog.middle.block211 ]
  br label %.lr.ph15.i.i.i73

vec.epilog.ph213:                                 ; preds = %vector.main.loop.iter.check201, %vec.epilog.iter.check214
  %vec.epilog.resume.val217 = phi i64 [ %n.vec204, %vec.epilog.iter.check214 ], [ 0, %vector.main.loop.iter.check201 ]
  %n.vec219 = and i64 %smax195, 9223372036854775800
  br label %vec.epilog.vector.body221

vec.epilog.vector.body221:                        ; preds = %vec.epilog.vector.body221, %vec.epilog.ph213
  %index222 = phi i64 [ %vec.epilog.resume.val217, %vec.epilog.ph213 ], [ %index.next224, %vec.epilog.vector.body221 ]
  %227 = getelementptr i8, ptr %218, i64 %index222
  %228 = getelementptr i8, ptr %209, i64 %index222
  %wide.load223 = load <8 x i8>, ptr %228, align 1
  store <8 x i8> %wide.load223, ptr %227, align 1
  %index.next224 = add nuw i64 %index222, 8
  %229 = icmp eq i64 %index.next224, %n.vec219
  br i1 %229, label %vec.epilog.middle.block211, label %vec.epilog.vector.body221, !llvm.loop !33

vec.epilog.middle.block211:                       ; preds = %vec.epilog.vector.body221
  %cmp.n225 = icmp eq i64 %smax195, %n.vec219
  br i1 %cmp.n225, label %.preheader.i.i.i71, label %.lr.ph15.i.i.i73.preheader

.preheader.i.i.i71:                               ; preds = %.lr.ph15.i.i.i73, %vec.epilog.middle.block211, %middle.block196
  tail call void @free(ptr nonnull %209)
  store i64 %217, ptr %8, align 8
  store ptr %218, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit78

.lr.ph15.i.i.i73:                                 ; preds = %.lr.ph15.i.i.i73.preheader, %.lr.ph15.i.i.i73
  %.114.i.i.i74 = phi i64 [ %233, %.lr.ph15.i.i.i73 ], [ %.114.i.i.i74.ph, %.lr.ph15.i.i.i73.preheader ]
  %230 = getelementptr i8, ptr %218, i64 %.114.i.i.i74
  %231 = getelementptr i8, ptr %209, i64 %.114.i.i.i74
  %232 = load i8, ptr %231, align 1
  store i8 %232, ptr %230, align 1
  %233 = add nuw nsw i64 %.114.i.i.i74, 1
  %234 = icmp slt i64 %233, %205
  br i1 %234, label %.lr.ph15.i.i.i73, label %.preheader.i.i.i71, !llvm.loop !34

rl_m_append__String_int8_t.exit78:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68, %.preheader.i.i.i71
  %235 = phi i64 [ %217, %.preheader.i.i.i71 ], [ %214, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68 ]
  %236 = phi ptr [ %218, %.preheader.i.i.i71 ], [ %209, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i68 ]
  %237 = ptrtoint ptr %236 to i64
  %238 = getelementptr i8, ptr %236, i64 %205
  store i8 0, ptr %238, align 1
  %.not151 = icmp eq i64 %205, 9223372036854775807
  br i1 %.not151, label %239, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79

239:                                              ; preds = %rl_m_append__String_int8_t.exit78
  %240 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
  tail call void @llvm.trap()
  unreachable

rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79:   ; preds = %rl_m_append__String_int8_t.exit78
  %241 = getelementptr i8, ptr %236, i64 %213
  %242 = getelementptr i8, ptr %241, i64 -1
  store i8 10, ptr %242, align 1
  %243 = add nuw i64 %205, 2
  %244 = icmp sgt i64 %235, %243
  br i1 %244, label %rl_m_append__String_int8_t.exit89, label %245

245:                                              ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79
  %246 = shl i64 %243, 1
  %247 = tail call ptr @malloc(i64 %246)
  %248 = ptrtoint ptr %247 to i64
  %249 = icmp sgt i64 %246, 0
  br i1 %249, label %.lr.ph.preheader.i.i.i86, label %iter.check166

.lr.ph.preheader.i.i.i86:                         ; preds = %245
  tail call void @llvm.memset.p0.i64(ptr align 1 %247, i8 0, i64 %246, i1 false)
  br label %iter.check166

iter.check166:                                    ; preds = %.lr.ph.preheader.i.i.i86, %245
  %smax162 = tail call i64 @llvm.smax.i64(i64 %213, i64 1)
  %min.iters.check164 = icmp slt i64 %213, 8
  %250 = sub i64 %248, %237
  %diff.check161 = icmp ult i64 %250, 32
  %or.cond364 = or i1 %min.iters.check164, %diff.check161
  br i1 %or.cond364, label %.lr.ph15.i.i.i84.preheader, label %vector.main.loop.iter.check168

vector.main.loop.iter.check168:                   ; preds = %iter.check166
  %min.iters.check167 = icmp slt i64 %213, 32
  br i1 %min.iters.check167, label %vec.epilog.ph180, label %vector.ph169

vector.ph169:                                     ; preds = %vector.main.loop.iter.check168
  %n.vec171 = and i64 %smax162, 9223372036854775776
  br label %vector.body172

vector.body172:                                   ; preds = %vector.body172, %vector.ph169
  %index173 = phi i64 [ 0, %vector.ph169 ], [ %index.next176, %vector.body172 ]
  %251 = getelementptr i8, ptr %247, i64 %index173
  %252 = getelementptr i8, ptr %236, i64 %index173
  %253 = getelementptr i8, ptr %252, i64 16
  %wide.load174 = load <16 x i8>, ptr %252, align 1
  %wide.load175 = load <16 x i8>, ptr %253, align 1
  %254 = getelementptr i8, ptr %251, i64 16
  store <16 x i8> %wide.load174, ptr %251, align 1
  store <16 x i8> %wide.load175, ptr %254, align 1
  %index.next176 = add nuw i64 %index173, 32
  %255 = icmp eq i64 %index.next176, %n.vec171
  br i1 %255, label %middle.block163, label %vector.body172, !llvm.loop !35

middle.block163:                                  ; preds = %vector.body172
  %cmp.n177 = icmp eq i64 %smax162, %n.vec171
  br i1 %cmp.n177, label %.preheader.i.i.i82, label %vec.epilog.iter.check181

vec.epilog.iter.check181:                         ; preds = %middle.block163
  %n.vec.remaining182 = and i64 %smax162, 24
  %min.epilog.iters.check183 = icmp eq i64 %n.vec.remaining182, 0
  br i1 %min.epilog.iters.check183, label %.lr.ph15.i.i.i84.preheader, label %vec.epilog.ph180

.lr.ph15.i.i.i84.preheader:                       ; preds = %vec.epilog.middle.block178, %iter.check166, %vec.epilog.iter.check181
  %.114.i.i.i85.ph = phi i64 [ 0, %iter.check166 ], [ %n.vec171, %vec.epilog.iter.check181 ], [ %n.vec186, %vec.epilog.middle.block178 ]
  br label %.lr.ph15.i.i.i84

vec.epilog.ph180:                                 ; preds = %vector.main.loop.iter.check168, %vec.epilog.iter.check181
  %vec.epilog.resume.val184 = phi i64 [ %n.vec171, %vec.epilog.iter.check181 ], [ 0, %vector.main.loop.iter.check168 ]
  %n.vec186 = and i64 %smax162, 9223372036854775800
  br label %vec.epilog.vector.body188

vec.epilog.vector.body188:                        ; preds = %vec.epilog.vector.body188, %vec.epilog.ph180
  %index189 = phi i64 [ %vec.epilog.resume.val184, %vec.epilog.ph180 ], [ %index.next191, %vec.epilog.vector.body188 ]
  %256 = getelementptr i8, ptr %247, i64 %index189
  %257 = getelementptr i8, ptr %236, i64 %index189
  %wide.load190 = load <8 x i8>, ptr %257, align 1
  store <8 x i8> %wide.load190, ptr %256, align 1
  %index.next191 = add nuw i64 %index189, 8
  %258 = icmp eq i64 %index.next191, %n.vec186
  br i1 %258, label %vec.epilog.middle.block178, label %vec.epilog.vector.body188, !llvm.loop !36

vec.epilog.middle.block178:                       ; preds = %vec.epilog.vector.body188
  %cmp.n192 = icmp eq i64 %smax162, %n.vec186
  br i1 %cmp.n192, label %.preheader.i.i.i82, label %.lr.ph15.i.i.i84.preheader

.preheader.i.i.i82:                               ; preds = %.lr.ph15.i.i.i84, %vec.epilog.middle.block178, %middle.block163
  tail call void @free(ptr nonnull %236)
  store i64 %246, ptr %8, align 8
  store ptr %247, ptr %6, align 8
  br label %rl_m_append__String_int8_t.exit89

.lr.ph15.i.i.i84:                                 ; preds = %.lr.ph15.i.i.i84.preheader, %.lr.ph15.i.i.i84
  %.114.i.i.i85 = phi i64 [ %262, %.lr.ph15.i.i.i84 ], [ %.114.i.i.i85.ph, %.lr.ph15.i.i.i84.preheader ]
  %259 = getelementptr i8, ptr %247, i64 %.114.i.i.i85
  %260 = getelementptr i8, ptr %236, i64 %.114.i.i.i85
  %261 = load i8, ptr %260, align 1
  store i8 %261, ptr %259, align 1
  %262 = add nuw nsw i64 %.114.i.i.i85, 1
  %263 = icmp slt i64 %262, %213
  br i1 %263, label %.lr.ph15.i.i.i84, label %.preheader.i.i.i82, !llvm.loop !37

rl_m_append__String_int8_t.exit89:                ; preds = %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79, %.preheader.i.i.i82
  %264 = phi ptr [ %247, %.preheader.i.i.i82 ], [ %236, %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i79 ]
  %265 = getelementptr i8, ptr %264, i64 %213
  store i8 0, ptr %265, align 1
  store i64 %243, ptr %7, align 8
  %266 = add i64 %.0140148, 1
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %2)
  %.not2.i90 = icmp eq i64 %266, 0
  br i1 %.not2.i90, label %rl__indent_string__String_int64_t.exit94, label %.lr.ph.i91

.lr.ph.i91:                                       ; preds = %rl_m_append__String_int8_t.exit89, %.lr.ph.i91
  %.03.i92 = phi i64 [ %267, %.lr.ph.i91 ], [ 0, %rl_m_append__String_int8_t.exit89 ]
  store ptr @str_13, ptr %2, align 8
  call void @rl_m_append__String_strlit(ptr nonnull %6, ptr nonnull %2)
  %267 = add nuw i64 %.03.i92, 1
  %.not.i93 = icmp eq i64 %.03.i92, %.0140148
  br i1 %.not.i93, label %rl__indent_string__String_int64_t.exit94, label %.lr.ph.i91

rl__indent_string__String_int64_t.exit94:         ; preds = %.lr.ph.i91, %rl_m_append__String_int8_t.exit89
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %2)
  br label %268

268:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit38, %rl_m_append__String_int8_t.exit66, %rl_m_append__String_int8_t.exit, %rl__indent_string__String_int64_t.exit94
  %.1141 = phi i64 [ %.0140148, %rl_m_append__String_int8_t.exit ], [ %164, %rl_m_append__String_int8_t.exit66 ], [ %266, %rl__indent_string__String_int64_t.exit94 ], [ %.0140148, %rl_m_get__String_int64_t_r_int8_tRef.exit38 ]
  %.1 = phi i64 [ %.0149, %rl_m_append__String_int8_t.exit ], [ %.0149, %rl_m_append__String_int8_t.exit66 ], [ %.0149, %rl__indent_string__String_int64_t.exit94 ], [ %spec.select, %rl_m_get__String_int64_t_r_int8_tRef.exit38 ]
  %269 = add i64 %.1, 1
  %270 = load i64, ptr %10, align 8
  %271 = add i64 %270, -1
  %272 = icmp slt i64 %269, %271
  br i1 %272, label %.lr.ph, label %._crit_edge.loopexit

._crit_edge.loopexit:                             ; preds = %268
  %.pre = load i64, ptr %8, align 8
  %273 = icmp eq i64 %.pre, 0
  %274 = getelementptr inbounds i8, ptr %5, i64 8
  %275 = getelementptr inbounds i8, ptr %5, i64 16
  store i64 4, ptr %275, align 8
  %276 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %276, ptr %5, align 8
  store i32 0, ptr %276, align 1
  store i64 1, ptr %274, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %5, ptr nonnull readonly %6)
  br i1 %273, label %rl_m_drop__String.exit, label %280

.critedge:                                        ; preds = %rl_m_init__String.exit
  %277 = getelementptr inbounds i8, ptr %5, i64 8
  %278 = getelementptr inbounds i8, ptr %5, i64 16
  store i64 4, ptr %278, align 8
  %279 = tail call dereferenceable_or_null(4) ptr @malloc(i64 4)
  store ptr %279, ptr %5, align 8
  store i32 0, ptr %279, align 1
  store i64 1, ptr %277, align 8
  call void @rl_m_assign__VectorTint8_tT_VectorTint8_tT(ptr nonnull %5, ptr nonnull readonly %6)
  br label %280

280:                                              ; preds = %.critedge, %._crit_edge.loopexit
  %281 = load ptr, ptr %6, align 8
  tail call void @free(ptr %281)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %._crit_edge.loopexit, %280
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
  br i1 %or.cond.not, label %rl_m_equal__String_String_r_bool.exit, label %.lr.ph.i, !llvm.loop !38

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
  %.not3.i.i = icmp eq i64 %11, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %12

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
  br i1 %27, label %middle.block, label %vector.body, !llvm.loop !40

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
  br i1 %30, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !41

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
  br i1 %35, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !42

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
  br i1 %69, label %middle.block97, label %vector.body106, !llvm.loop !43

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
  br i1 %72, label %vec.epilog.middle.block112, label %vec.epilog.vector.body122, !llvm.loop !44

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
  br i1 %77, label %.lr.ph15.i.i5, label %.preheader.i.i3, !llvm.loop !45

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
  br i1 %105, label %middle.block64, label %vector.body73, !llvm.loop !46

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
  br i1 %108, label %vec.epilog.middle.block79, label %vec.epilog.vector.body89, !llvm.loop !47

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
  br i1 %113, label %.lr.ph15.i.i16, label %.preheader.i.i14, !llvm.loop !48

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
  br i1 %139, label %middle.block130, label %vector.body139, !llvm.loop !49

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
  br i1 %142, label %vec.epilog.middle.block145, label %vec.epilog.vector.body155, !llvm.loop !50

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
  br i1 %147, label %.lr.ph15.i.i26, label %.preheader.i.i24, !llvm.loop !51

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
  br i1 %167, label %middle.block163, label %vector.body172, !llvm.loop !52

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
  br i1 %170, label %vec.epilog.middle.block178, label %vec.epilog.vector.body188, !llvm.loop !53

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
  br i1 %175, label %.lr.ph15.i.i36, label %.preheader.i.i34, !llvm.loop !54

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
  br i1 %38, label %middle.block, label %vector.body, !llvm.loop !55

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
  br i1 %41, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !56

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
  br i1 %46, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !57

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
  br i1 %73, label %middle.block28, label %vector.body37, !llvm.loop !58

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
  br i1 %76, label %vec.epilog.middle.block43, label %vec.epilog.vector.body53, !llvm.loop !59

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
  br i1 %81, label %.lr.ph15.i.i5, label %.preheader.i.i3, !llvm.loop !60

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
  br i1 %32, label %middle.block, label %vector.body, !llvm.loop !61

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
  br i1 %35, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !62

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
  br i1 %40, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !63

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
  br i1 %67, label %middle.block29, label %vector.body38, !llvm.loop !64

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
  br i1 %70, label %vec.epilog.middle.block44, label %vec.epilog.vector.body54, !llvm.loop !65

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
  br i1 %75, label %.lr.ph15.i.i7, label %.preheader.i.i5, !llvm.loop !66

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
  br i1 %16, label %middle.block, label %vector.body, !llvm.loop !67

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
  br i1 %.not, label %._crit_edge, label %.lr.ph, !llvm.loop !68

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
  br i1 %28, label %middle.block, label %vector.body, !llvm.loop !69

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
  br i1 %31, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !70

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
  br i1 %36, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !71

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
  br i1 %24, label %middle.block, label %vector.body, !llvm.loop !72

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
  br i1 %27, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !73

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
  br i1 %32, label %.lr.ph15.i.i, label %.preheader.i.i, !llvm.loop !74

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
  %.not3.i.i = icmp eq i64 %10, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %11

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
  %.not3.i.i = icmp eq i64 %10, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %11

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
  store ptr @str_16, ptr %8, align 8
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
  %.not3.i.i.i = icmp eq i64 %15, 0
  br i1 %.not3.i.i.i, label %rl_s__strlit_r_String.exit, label %16

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
  %.not3.i.i = icmp eq i64 %19, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %20

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
  %26 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

._crit_edge:                                      ; preds = %34, %.lr.ph.i
  %27 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %.not9.i = icmp ult i64 %29, %33
  br i1 %.not9.i, label %34, label %rl__consume_space__String_int64_t.exit

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
  %44 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

45:                                               ; preds = %.lr.ph.i6.preheader
  %46 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit.i7:     ; preds = %.lr.ph.i6.preheader
  %.not7.i = icmp eq i8 %32, 34
  br i1 %.not7.i, label %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit, label %common.ret

common.ret:                                       ; preds = %.backedge, %62, %rl__consume_space__String_int64_t.exit, %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit.i7, %122
  %.sink = phi i8 [ 1, %122 ], [ 0, %rl_m_get__String_int64_t_r_int8_tRef.exit.i7 ], [ 0, %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit ], [ 0, %rl__consume_space__String_int64_t.exit ], [ 0, %62 ], [ 0, %.backedge ]
  store i8 %.sink, ptr %0, align 1
  ret void

.lr.ph:                                           ; preds = %rl_m_substring_matches__String_strlit_int64_t_r_bool.exit
  %47 = getelementptr i8, ptr %1, i64 8
  %48 = getelementptr i8, ptr %1, i64 16
  br label %49

49:                                               ; preds = %.lr.ph, %.backedge
  %50 = phi i64 [ %41, %.lr.ph ], [ %112, %.backedge ]
  %51 = phi i64 [ %40, %.lr.ph ], [ %storemerge, %.backedge ]
  %52 = icmp sgt i64 %51, -1
  br i1 %52, label %55, label %53

53:                                               ; preds = %49
  %54 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_10)
  tail call void @llvm.trap()
  unreachable

55:                                               ; preds = %49
  %56 = icmp slt i64 %51, %50
  br i1 %56, label %rl_m_get__String_int64_t_r_int8_tRef.exit, label %57

57:                                               ; preds = %55
  %58 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit:        ; preds = %55
  %59 = load ptr, ptr %2, align 8
  %60 = getelementptr i8, ptr %59, i64 %51
  %61 = load i8, ptr %60, align 1
  switch i8 %61, label %114 [
    i8 34, label %122
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
  %70 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit9:       ; preds = %67
  %71 = load ptr, ptr %2, align 8
  %72 = getelementptr i8, ptr %71, i64 %63
  %73 = load i8, ptr %72, align 1
  %74 = icmp eq i8 %73, 34
  br i1 %74, label %75, label %114

75:                                               ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit9
  %76 = load i64, ptr %47, align 8
  %77 = icmp sgt i64 %76, 0
  br i1 %77, label %rl_m_back__VectorTint8_tT_r_int8_tRef.exit.i, label %78

78:                                               ; preds = %75
  %79 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_12)
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
  br i1 %91, label %.lr.ph.preheader.i.i.i, label %.preheader12.i.i.i

.lr.ph.preheader.i.i.i:                           ; preds = %87
  tail call void @llvm.memset.p0.i64(ptr align 1 %89, i8 0, i64 %88, i1 false)
  br label %.preheader12.i.i.i

.preheader12.i.i.i:                               ; preds = %.lr.ph.preheader.i.i.i, %87
  %92 = icmp sgt i64 %83, 0
  %.pre.i.i.i = load ptr, ptr %1, align 8
  br i1 %92, label %iter.check, label %.preheader.i.i.i

iter.check:                                       ; preds = %.preheader12.i.i.i
  %.pre.i.i.i77 = ptrtoint ptr %.pre.i.i.i to i64
  %min.iters.check = icmp ult i64 %83, 8
  %93 = sub i64 %90, %.pre.i.i.i77
  %diff.check = icmp ult i64 %93, 32
  %or.cond = select i1 %min.iters.check, i1 true, i1 %diff.check
  br i1 %or.cond, label %.lr.ph15.i.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check78 = icmp ult i64 %83, 32
  br i1 %min.iters.check78, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %83, 9223372036854775776
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %94 = getelementptr i8, ptr %89, i64 %index
  %95 = getelementptr i8, ptr %.pre.i.i.i, i64 %index
  %96 = getelementptr i8, ptr %95, i64 16
  %wide.load = load <16 x i8>, ptr %95, align 1
  %wide.load79 = load <16 x i8>, ptr %96, align 1
  %97 = getelementptr i8, ptr %94, i64 16
  store <16 x i8> %wide.load, ptr %94, align 1
  store <16 x i8> %wide.load79, ptr %97, align 1
  %index.next = add nuw i64 %index, 32
  %98 = icmp eq i64 %index.next, %n.vec
  br i1 %98, label %middle.block, label %vector.body, !llvm.loop !75

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %83, %n.vec
  br i1 %cmp.n, label %.preheader.i.i.i, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %83, 24
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph15.i.i.i.preheader, label %vec.epilog.ph

.lr.ph15.i.i.i.preheader:                         ; preds = %vec.epilog.middle.block, %iter.check, %vec.epilog.iter.check
  %.114.i.i.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec81, %vec.epilog.middle.block ]
  br label %.lr.ph15.i.i.i

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec81 = and i64 %83, 9223372036854775800
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index82 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next84, %vec.epilog.vector.body ]
  %99 = getelementptr i8, ptr %89, i64 %index82
  %100 = getelementptr i8, ptr %.pre.i.i.i, i64 %index82
  %wide.load83 = load <8 x i8>, ptr %100, align 1
  store <8 x i8> %wide.load83, ptr %99, align 1
  %index.next84 = add nuw i64 %index82, 8
  %101 = icmp eq i64 %index.next84, %n.vec81
  br i1 %101, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !76

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n85 = icmp eq i64 %83, %n.vec81
  br i1 %cmp.n85, label %.preheader.i.i.i, label %.lr.ph15.i.i.i.preheader

.preheader.i.i.i:                                 ; preds = %.lr.ph15.i.i.i, %middle.block, %vec.epilog.middle.block, %.preheader12.i.i.i
  tail call void @free(ptr %.pre.i.i.i)
  store i64 %88, ptr %48, align 8
  store ptr %89, ptr %1, align 8
  %.pre.i.i = load i64, ptr %47, align 8
  br label %rl_m_append__String_int8_t.exit

.lr.ph15.i.i.i:                                   ; preds = %.lr.ph15.i.i.i.preheader, %.lr.ph15.i.i.i
  %.114.i.i.i = phi i64 [ %105, %.lr.ph15.i.i.i ], [ %.114.i.i.i.ph, %.lr.ph15.i.i.i.preheader ]
  %102 = getelementptr i8, ptr %89, i64 %.114.i.i.i
  %103 = getelementptr i8, ptr %.pre.i.i.i, i64 %.114.i.i.i
  %104 = load i8, ptr %103, align 1
  store i8 %104, ptr %102, align 1
  %105 = add nuw nsw i64 %.114.i.i.i, 1
  %106 = icmp slt i64 %105, %83
  br i1 %106, label %.lr.ph15.i.i.i, label %.preheader.i.i.i, !llvm.loop !77

rl_m_append__String_int8_t.exit:                  ; preds = %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i, %.preheader.i.i.i
  %107 = phi ptr [ %.pre2.i.i, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i ], [ %89, %.preheader.i.i.i ]
  %108 = phi i64 [ %83, %.rl_m__grow__VectorTint8_tT_int64_t.exit_crit_edge.i.i ], [ %.pre.i.i, %.preheader.i.i.i ]
  %109 = getelementptr i8, ptr %107, i64 %108
  store i8 0, ptr %109, align 1
  %110 = load i64, ptr %47, align 8
  %111 = add i64 %110, 1
  store i64 %111, ptr %47, align 8
  br label %.backedge

.backedge:                                        ; preds = %rl_m_append__String_int8_t.exit, %rl_m_get__String_int64_t_r_int8_tRef.exit10
  %storemerge.in = load i64, ptr %3, align 8
  %storemerge = add i64 %storemerge.in, 1
  store i64 %storemerge, ptr %3, align 8
  %112 = load i64, ptr %23, align 8
  %113 = add i64 %112, -2
  %.not5 = icmp eq i64 %storemerge.in, %113
  br i1 %.not5, label %common.ret, label %49

114:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit9, %rl_m_get__String_int64_t_r_int8_tRef.exit
  %115 = phi ptr [ %59, %rl_m_get__String_int64_t_r_int8_tRef.exit ], [ %71, %rl_m_get__String_int64_t_r_int8_tRef.exit9 ]
  %116 = phi i64 [ %50, %rl_m_get__String_int64_t_r_int8_tRef.exit ], [ %64, %rl_m_get__String_int64_t_r_int8_tRef.exit9 ]
  %117 = phi i64 [ %51, %rl_m_get__String_int64_t_r_int8_tRef.exit ], [ %63, %rl_m_get__String_int64_t_r_int8_tRef.exit9 ]
  %118 = icmp ult i64 %117, %116
  br i1 %118, label %rl_m_get__String_int64_t_r_int8_tRef.exit10, label %119

119:                                              ; preds = %114
  %120 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  tail call void @llvm.trap()
  unreachable

rl_m_get__String_int64_t_r_int8_tRef.exit10:      ; preds = %114
  %121 = getelementptr i8, ptr %115, i64 %117
  tail call void @rl_m_append__String_int8_t(ptr %1, ptr %121)
  br label %.backedge

122:                                              ; preds = %rl_m_get__String_int64_t_r_int8_tRef.exit
  %123 = add nuw nsw i64 %51, 1
  store i64 %123, ptr %3, align 8
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
  %.not22.i = icmp sgt i64 %99, 0
  br i1 %.not22.i, label %.lr.ph.i8, label %._crit_edge.i

.lr.ph.i8:                                        ; preds = %90
  %100 = load ptr, ptr %53, align 8
  br label %102

._crit_edge.i:                                    ; preds = %90, %119
  %101 = call i32 @puts(ptr nonnull dereferenceable(1) @str_3)
  call void @llvm.trap()
  unreachable

102:                                              ; preds = %119, %.lr.ph.i8
  %.pn.i = phi i64 [ %98, %.lr.ph.i8 ], [ %120, %119 ]
  %.01124.i = phi i64 [ 0, %.lr.ph.i8 ], [ %103, %119 ]
  %.025.i = srem i64 %.pn.i, %99
  %103 = add nuw nsw i64 %.01124.i, 1
  %104 = getelementptr %Entry, ptr %100, i64 %.025.i
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
  %.not20.i = icmp eq i64 %113, %88
  br i1 %.not20.i, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit, label %.thread.i

.thread.i:                                        ; preds = %111, %107
  %114 = add i64 %.025.i, %99
  %115 = srem i64 %109, %99
  %116 = sub i64 %114, %115
  %117 = srem i64 %116, %99
  %118 = icmp slt i64 %117, %.01124.i
  br i1 %118, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %119

119:                                              ; preds = %.thread.i
  %120 = add i64 %.025.i, 1
  %.not.i = icmp slt i64 %103, %99
  br i1 %.not.i, label %102, label %._crit_edge.i

rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit: ; preds = %111
  store ptr @str_18, ptr %48, align 8
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
  %.not3.i.i.i = icmp eq i64 %123, 0
  br i1 %.not3.i.i.i, label %rl_s__strlit_r_String.exit, label %124

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
  store ptr @str_19, ptr %44, align 8
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
  %.not3.i.i.i9 = icmp eq i64 %128, 0
  br i1 %.not3.i.i.i9, label %rl_s__strlit_r_String.exit10, label %129

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
  %.not3.i.i = icmp eq i64 %131, 0
  br i1 %.not3.i.i, label %rl_m_drop__String.exit, label %132

132:                                              ; preds = %rl_s__strlit_r_String.exit10
  %133 = load ptr, ptr %47, align 8
  call void @free(ptr %133)
  br label %rl_m_drop__String.exit

rl_m_drop__String.exit:                           ; preds = %rl_s__strlit_r_String.exit10, %132
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %70, i8 0, i64 16, i1 false)
  %134 = load i64, ptr %71, align 8
  %.not3.i.i11 = icmp eq i64 %134, 0
  br i1 %.not3.i.i11, label %rl_m_drop__String.exit12, label %135

135:                                              ; preds = %rl_m_drop__String.exit
  %136 = load ptr, ptr %46, align 8
  call void @free(ptr %136)
  br label %rl_m_drop__String.exit12

rl_m_drop__String.exit12:                         ; preds = %rl_m_drop__String.exit, %135
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %72, i8 0, i64 16, i1 false)
  %137 = load i64, ptr %73, align 8
  %.not3.i.i13 = icmp eq i64 %137, 0
  br i1 %.not3.i.i13, label %rl_m_drop__String.exit14, label %138

138:                                              ; preds = %rl_m_drop__String.exit12
  %139 = load ptr, ptr %45, align 8
  call void @free(ptr %139)
  br label %rl_m_drop__String.exit14

rl_m_drop__String.exit14:                         ; preds = %rl_m_drop__String.exit12, %138
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %74, i8 0, i64 16, i1 false)
  %140 = load i64, ptr %75, align 8
  %.not3.i.i15 = icmp eq i64 %140, 0
  br i1 %.not3.i.i15, label %rl_m_drop__String.exit16, label %141

141:                                              ; preds = %rl_m_drop__String.exit14
  %142 = load ptr, ptr %43, align 8
  call void @free(ptr %142)
  br label %rl_m_drop__String.exit16

rl_m_drop__String.exit16:                         ; preds = %rl_m_drop__String.exit14, %141
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %76, i8 0, i64 16, i1 false)
  %143 = load i64, ptr %77, align 8
  %.not3.i.i17 = icmp eq i64 %143, 0
  br i1 %.not3.i.i17, label %rl_m_drop__String.exit18, label %144

144:                                              ; preds = %rl_m_drop__String.exit16
  %145 = load ptr, ptr %42, align 8
  call void @free(ptr %145)
  br label %rl_m_drop__String.exit18

rl_m_drop__String.exit18:                         ; preds = %rl_m_drop__String.exit16, %144
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %78, i8 0, i64 16, i1 false)
  %146 = load i64, ptr %79, align 8
  %.not3.i.i19 = icmp eq i64 %146, 0
  br i1 %.not3.i.i19, label %rl_m_drop__String.exit20, label %147

147:                                              ; preds = %rl_m_drop__String.exit18
  %148 = load ptr, ptr %40, align 8
  call void @free(ptr %148)
  br label %rl_m_drop__String.exit20

rl_m_drop__String.exit20:                         ; preds = %rl_m_drop__String.exit18, %147
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %80, i8 0, i64 16, i1 false)
  %149 = load i64, ptr %81, align 8
  %.not3.i.i21 = icmp eq i64 %149, 0
  br i1 %.not3.i.i21, label %rl_m_drop__String.exit22, label %150

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
  %.not22.i24 = icmp sgt i64 %164, 0
  br i1 %.not22.i24, label %.lr.ph.i26, label %._crit_edge.i25

.lr.ph.i26:                                       ; preds = %154
  %165 = load ptr, ptr %53, align 8
  br label %167

._crit_edge.i25:                                  ; preds = %154, %184
  %166 = call i32 @puts(ptr nonnull dereferenceable(1) @str_3)
  call void @llvm.trap()
  unreachable

167:                                              ; preds = %184, %.lr.ph.i26
  %.pn.i27 = phi i64 [ %163, %.lr.ph.i26 ], [ %185, %184 ]
  %.01124.i28 = phi i64 [ 0, %.lr.ph.i26 ], [ %168, %184 ]
  %.025.i29 = srem i64 %.pn.i27, %164
  %168 = add nuw nsw i64 %.01124.i28, 1
  %169 = getelementptr %Entry, ptr %165, i64 %.025.i29
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
  %.not20.i33 = icmp eq i64 %178, %155
  br i1 %.not20.i33, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34, label %.thread.i30

.thread.i30:                                      ; preds = %176, %172
  %179 = add i64 %.025.i29, %164
  %180 = srem i64 %174, %164
  %181 = sub i64 %179, %180
  %182 = srem i64 %181, %164
  %183 = icmp slt i64 %182, %.01124.i28
  br i1 %183, label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread, label %184

184:                                              ; preds = %.thread.i30
  %185 = add i64 %.025.i29, 1
  %.not.i31 = icmp slt i64 %168, %164
  br i1 %.not.i31, label %167, label %._crit_edge.i25

rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34: ; preds = %176
  store ptr @str_20, ptr %37, align 8
  call void @rl_print_string_lit__strlit(ptr nonnull %37)
  br label %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread

rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit.thread: ; preds = %102, %.thread.i, %167, %.thread.i30, %rl_m_drop__String.exit22, %85, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34
  %186 = phi i64 [ 0, %rl_m_drop__String.exit22 ], [ 0, %85 ], [ %152, %rl_m_contains__DictTint64_tTint64_tT_int64_t_r_bool.exit34 ], [ %152, %.thread.i30 ], [ %152, %167 ], [ %86, %.thread.i ], [ %86, %102 ]
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
  %226 = call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  call void @llvm.trap()
  unreachable

227:                                              ; preds = %222
  %228 = icmp slt i64 %.0120134, %195
  br i1 %228, label %rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit39, label %229

229:                                              ; preds = %227
  %230 = call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
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
  %236 = call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  call void @llvm.trap()
  unreachable

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit40: ; preds = %233
  %237 = load ptr, ptr %36, align 8
  %238 = getelementptr i64, ptr %237, i64 %.0118136
  %239 = add nuw nsw i64 %.0118136, 1
  store ptr @str_21, ptr %32, align 8
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
  %.not3.i.i.i41 = icmp eq i64 %242, 0
  br i1 %.not3.i.i.i41, label %rl_s__strlit_r_String.exit42, label %243

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
  store ptr @str_22, ptr %28, align 8
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
  %.not3.i.i.i43 = icmp eq i64 %247, 0
  br i1 %.not3.i.i.i43, label %rl_s__strlit_r_String.exit44, label %248

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
  %.not3.i.i45 = icmp eq i64 %250, 0
  br i1 %.not3.i.i45, label %rl_m_drop__String.exit46, label %251

251:                                              ; preds = %rl_s__strlit_r_String.exit44
  %252 = load ptr, ptr %31, align 8
  call void @free(ptr %252)
  br label %rl_m_drop__String.exit46

rl_m_drop__String.exit46:                         ; preds = %rl_s__strlit_r_String.exit44, %251
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %209, i8 0, i64 16, i1 false)
  %253 = load i64, ptr %210, align 8
  %.not3.i.i47 = icmp eq i64 %253, 0
  br i1 %.not3.i.i47, label %rl_m_drop__String.exit48, label %254

254:                                              ; preds = %rl_m_drop__String.exit46
  %255 = load ptr, ptr %30, align 8
  call void @free(ptr %255)
  br label %rl_m_drop__String.exit48

rl_m_drop__String.exit48:                         ; preds = %rl_m_drop__String.exit46, %254
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %211, i8 0, i64 16, i1 false)
  %256 = load i64, ptr %212, align 8
  %.not3.i.i49 = icmp eq i64 %256, 0
  br i1 %.not3.i.i49, label %rl_m_drop__String.exit50, label %257

257:                                              ; preds = %rl_m_drop__String.exit48
  %258 = load ptr, ptr %29, align 8
  call void @free(ptr %258)
  br label %rl_m_drop__String.exit50

rl_m_drop__String.exit50:                         ; preds = %rl_m_drop__String.exit48, %257
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %213, i8 0, i64 16, i1 false)
  %259 = load i64, ptr %214, align 8
  %.not3.i.i51 = icmp eq i64 %259, 0
  br i1 %.not3.i.i51, label %rl_m_drop__String.exit52, label %260

260:                                              ; preds = %rl_m_drop__String.exit50
  %261 = load ptr, ptr %27, align 8
  call void @free(ptr %261)
  br label %rl_m_drop__String.exit52

rl_m_drop__String.exit52:                         ; preds = %rl_m_drop__String.exit50, %260
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %215, i8 0, i64 16, i1 false)
  %262 = load i64, ptr %216, align 8
  %.not3.i.i53 = icmp eq i64 %262, 0
  br i1 %.not3.i.i53, label %rl_m_drop__String.exit54, label %263

263:                                              ; preds = %rl_m_drop__String.exit52
  %264 = load ptr, ptr %26, align 8
  call void @free(ptr %264)
  br label %rl_m_drop__String.exit54

rl_m_drop__String.exit54:                         ; preds = %rl_m_drop__String.exit52, %263
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %217, i8 0, i64 16, i1 false)
  %265 = load i64, ptr %218, align 8
  %.not3.i.i55 = icmp eq i64 %265, 0
  br i1 %.not3.i.i55, label %rl_m_drop__String.exit56, label %266

266:                                              ; preds = %rl_m_drop__String.exit54
  %267 = load ptr, ptr %24, align 8
  call void @free(ptr %267)
  br label %rl_m_drop__String.exit56

rl_m_drop__String.exit56:                         ; preds = %rl_m_drop__String.exit54, %266
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %219, i8 0, i64 16, i1 false)
  %268 = load i64, ptr %220, align 8
  %.not3.i.i57 = icmp eq i64 %268, 0
  br i1 %.not3.i.i57, label %rl_m_drop__String.exit58, label %269

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
  %298 = call i32 @puts(ptr nonnull dereferenceable(1) @str_11)
  call void @llvm.trap()
  unreachable

rl_m_get__VectorTint64_tT_int64_t_r_int64_tRef.exit59: ; preds = %295
  %299 = load ptr, ptr %35, align 8
  %300 = getelementptr i64, ptr %299, i64 %.0139
  %301 = add nuw nsw i64 %.0139, 1
  store ptr @str_23, ptr %22, align 8
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
  %.not3.i.i.i60 = icmp eq i64 %304, 0
  br i1 %.not3.i.i.i60, label %rl_s__strlit_r_String.exit61, label %305

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
  store ptr @str_22, ptr %18, align 8
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
  %.not3.i.i.i62 = icmp eq i64 %309, 0
  br i1 %.not3.i.i.i62, label %rl_s__strlit_r_String.exit63, label %310

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
  %.not3.i.i64 = icmp eq i64 %312, 0
  br i1 %.not3.i.i64, label %rl_m_drop__String.exit65, label %313

313:                                              ; preds = %rl_s__strlit_r_String.exit63
  %314 = load ptr, ptr %21, align 8
  call void @free(ptr %314)
  br label %rl_m_drop__String.exit65

rl_m_drop__String.exit65:                         ; preds = %rl_s__strlit_r_String.exit63, %313
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %282, i8 0, i64 16, i1 false)
  %315 = load i64, ptr %283, align 8
  %.not3.i.i66 = icmp eq i64 %315, 0
  br i1 %.not3.i.i66, label %rl_m_drop__String.exit67, label %316

316:                                              ; preds = %rl_m_drop__String.exit65
  %317 = load ptr, ptr %20, align 8
  call void @free(ptr %317)
  br label %rl_m_drop__String.exit67

rl_m_drop__String.exit67:                         ; preds = %rl_m_drop__String.exit65, %316
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %284, i8 0, i64 16, i1 false)
  %318 = load i64, ptr %285, align 8
  %.not3.i.i68 = icmp eq i64 %318, 0
  br i1 %.not3.i.i68, label %rl_m_drop__String.exit69, label %319

319:                                              ; preds = %rl_m_drop__String.exit67
  %320 = load ptr, ptr %19, align 8
  call void @free(ptr %320)
  br label %rl_m_drop__String.exit69

rl_m_drop__String.exit69:                         ; preds = %rl_m_drop__String.exit67, %319
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %286, i8 0, i64 16, i1 false)
  %321 = load i64, ptr %287, align 8
  %.not3.i.i70 = icmp eq i64 %321, 0
  br i1 %.not3.i.i70, label %rl_m_drop__String.exit71, label %322

322:                                              ; preds = %rl_m_drop__String.exit69
  %323 = load ptr, ptr %17, align 8
  call void @free(ptr %323)
  br label %rl_m_drop__String.exit71

rl_m_drop__String.exit71:                         ; preds = %rl_m_drop__String.exit69, %322
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %288, i8 0, i64 16, i1 false)
  %324 = load i64, ptr %289, align 8
  %.not3.i.i72 = icmp eq i64 %324, 0
  br i1 %.not3.i.i72, label %rl_m_drop__String.exit73, label %325

325:                                              ; preds = %rl_m_drop__String.exit71
  %326 = load ptr, ptr %16, align 8
  call void @free(ptr %326)
  br label %rl_m_drop__String.exit73

rl_m_drop__String.exit73:                         ; preds = %rl_m_drop__String.exit71, %325
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %290, i8 0, i64 16, i1 false)
  %327 = load i64, ptr %291, align 8
  %.not3.i.i74 = icmp eq i64 %327, 0
  br i1 %.not3.i.i74, label %rl_m_drop__String.exit75, label %328

328:                                              ; preds = %rl_m_drop__String.exit73
  %329 = load ptr, ptr %14, align 8
  call void @free(ptr %329)
  br label %rl_m_drop__String.exit75

rl_m_drop__String.exit75:                         ; preds = %rl_m_drop__String.exit73, %328
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %292, i8 0, i64 16, i1 false)
  %330 = load i64, ptr %293, align 8
  %.not3.i.i76 = icmp eq i64 %330, 0
  br i1 %.not3.i.i76, label %rl_m_drop__String.exit77, label %331

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
  %.not3.i = icmp eq i64 %338, 0
  br i1 %.not3.i, label %rl_m_drop__VectorTint64_tT.exit, label %339

339:                                              ; preds = %rl_m_drop__DictTint64_tTint64_tT.exit82
  %340 = load ptr, ptr %36, align 8
  call void @free(ptr %340)
  br label %rl_m_drop__VectorTint64_tT.exit

rl_m_drop__VectorTint64_tT.exit:                  ; preds = %rl_m_drop__DictTint64_tTint64_tT.exit82, %339
  %341 = getelementptr inbounds i8, ptr %35, i64 16
  %342 = load i64, ptr %341, align 8
  %.not3.i83 = icmp eq i64 %342, 0
  br i1 %.not3.i83, label %rl_m_drop__DictTint64_tTint64_tT.exit86, label %343

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

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.vector.reduce.add.v2i64(<2 x i64>) #13

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
!8 = distinct !{!8, !2, !3}
!9 = distinct !{!9, !2}
!10 = distinct !{!10, !2, !3}
!11 = distinct !{!11, !2}
!12 = distinct !{!12, !2, !3}
!13 = distinct !{!13, !2, !3}
!14 = distinct !{!14, !2}
!15 = distinct !{!15, !2, !3}
!16 = distinct !{!16, !2}
!17 = distinct !{!17, !2, !3}
!18 = distinct !{!18, !2, !3}
!19 = distinct !{!19, !2}
!20 = distinct !{!20, !2, !3}
!21 = distinct !{!21, !2, !3}
!22 = distinct !{!22, !2}
!23 = distinct !{!23, !2, !3}
!24 = distinct !{!24, !2, !3}
!25 = distinct !{!25, !2}
!26 = distinct !{!26, !2, !3}
!27 = distinct !{!27, !2, !3}
!28 = distinct !{!28, !2}
!29 = distinct !{!29, !2, !3}
!30 = distinct !{!30, !2, !3}
!31 = distinct !{!31, !2}
!32 = distinct !{!32, !2, !3}
!33 = distinct !{!33, !2, !3}
!34 = distinct !{!34, !2}
!35 = distinct !{!35, !2, !3}
!36 = distinct !{!36, !2, !3}
!37 = distinct !{!37, !2}
!38 = distinct !{!38, !39}
!39 = !{!"llvm.loop.peeled.count", i32 1}
!40 = distinct !{!40, !2, !3}
!41 = distinct !{!41, !2, !3}
!42 = distinct !{!42, !2}
!43 = distinct !{!43, !2, !3}
!44 = distinct !{!44, !2, !3}
!45 = distinct !{!45, !2}
!46 = distinct !{!46, !2, !3}
!47 = distinct !{!47, !2, !3}
!48 = distinct !{!48, !2}
!49 = distinct !{!49, !2, !3}
!50 = distinct !{!50, !2, !3}
!51 = distinct !{!51, !2}
!52 = distinct !{!52, !2, !3}
!53 = distinct !{!53, !2, !3}
!54 = distinct !{!54, !2}
!55 = distinct !{!55, !2, !3}
!56 = distinct !{!56, !2, !3}
!57 = distinct !{!57, !2}
!58 = distinct !{!58, !2, !3}
!59 = distinct !{!59, !2, !3}
!60 = distinct !{!60, !2}
!61 = distinct !{!61, !2, !3}
!62 = distinct !{!62, !2, !3}
!63 = distinct !{!63, !2}
!64 = distinct !{!64, !2, !3}
!65 = distinct !{!65, !2, !3}
!66 = distinct !{!66, !2}
!67 = distinct !{!67, !2, !3}
!68 = distinct !{!68, !3, !2}
!69 = distinct !{!69, !2, !3}
!70 = distinct !{!70, !2, !3}
!71 = distinct !{!71, !2}
!72 = distinct !{!72, !2, !3}
!73 = distinct !{!73, !2, !3}
!74 = distinct !{!74, !2}
!75 = distinct !{!75, !2, !3}
!76 = distinct !{!76, !2, !3}
!77 = distinct !{!77, !2}
