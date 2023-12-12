
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

define dso_local i32 @main()
{
    l_entry:
; logical-and begin
        %.ovkl = icmp eq i32 9, 0
    br i1 %.ovkl ,label %l_postAnd , label %l_andClause2
    l_andClause2:
; multiplication begin
        %.mgwo = mul i32 2, 3
; multipication end
        %.zeto = icmp ne i32 %.mgwo, 0
        %.vrwe = zext i1 %.zeto to i32
    br label %l_postAnd
    l_postAnd:
        %.pjwm = phi i32 [ 0 , %l_entry ], [ %.vrwe , %l_andClause2 ]
; logical-and end
    ret i32 %.pjwm
}


; MetaData section: this code was written by [Emeka Daniel] and he says thank you LLVM
!llvm.ident = !{!0}
!0 = !{!"(built by Emeka Daniel) llvm version 17.0.5"}
