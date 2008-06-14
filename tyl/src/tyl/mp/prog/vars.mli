(* Sections 4.2.2 & 4.2.3 *)

open Ast
open Util

val freeVarse : expr -> Id.Set.t
val freeVarsc : prop -> Id.Set.t
val freeVarsp : prog -> Id.Set.t

val isClosede : expr -> bool
val isClosedc : prop -> bool
val isClosedp : prog -> bool

val subee : expr -> Id.t -> expr -> expr
val subec : expr -> Id.t -> prop -> prop

