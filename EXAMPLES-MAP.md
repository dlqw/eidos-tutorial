# EXAMPLES-MAP（示例文件路径映射）

> 本表记录教程示例按主题重组前的旧路径与重组后的新路径，用于追溯 changelog 与历史文档中的引用。
> 重组日期：2026-08-02（分支 `docs/tutorial-restructure-2026-08-02`）。文件保持原编号，仅按主题归位。

## 主题目录

| 目录 | 主题 | 对应章节（拟） |
| --- | --- | --- |
| `examples/getting_started/` | 字面量与绑定 | 入门准备 |
| `examples/basics/` | 基础语法：函数/表达式/ADT/模块/错误处理 | 基础 1-6、11-14 章 |
| `examples/pattern/` | 模式匹配与覆盖分析 | 基础 7 章 |
| `examples/traits/` | Trait 与 named instance | 基础 9 章 |
| `examples/generics/` | 泛型、kind、const generics | 基础 8 章 / 进阶 3-4 章 |
| `examples/ownership/` | 借用与所有权契约 | 进阶 1 章 |
| `examples/effects/` | 效果系统 | 进阶 2 章 |
| `examples/functional/` | 函数式组合子与风格 | 进阶 5 章 |
| `examples/meta/` | 编译期元编程与 derive | 进阶 6 章 |
| `examples/ffi/` | FFI 与 C 互操作 | 进阶 8 章 |
| `examples/stdlib/` | 标准库使用 | 开发实践 |
| `examples/advanced/` | 重载与自定义运算符 | 进阶 9 章 |
| `examples/practice/` | 实战项目（文本统计工具） | 基础实战（新增，无旧路径） |
| `examples/build_host/` | BuildGraph 项目 | 进阶 7 章（独立验证） |

## 映射表

| 旧路径 | 新路径 |
| --- | --- |
| examples/59_custom_symbolic_operators.eidos | examples/advanced/59_custom_symbolic_operators.eidos |
| examples/67_function_overloads.eidos | examples/advanced/67_function_overloads.eidos |
| examples/02_functions_calls.eidos | examples/basics/02_functions_calls.eidos |
| examples/03_chain_method_calls.eidos | examples/basics/03_chain_method_calls.eidos |
| examples/04_adt_type_alias.eidos | examples/basics/04_adt_type_alias.eidos |
| examples/06_nested_call_parser_only.eidos | examples/basics/06_nested_call_parser_only.eidos |
| examples/07_block_result_known_issue.eidos | examples/basics/07_block_result_known_issue.eidos |
| examples/08_list_comprehension.eidos | examples/basics/08_list_comprehension.eidos |
| examples/12_let_pattern.eidos | examples/basics/12_let_pattern.eidos |
| examples/28_early_return.eidos | examples/basics/28_early_return.eidos |
| examples/30_curried_pattern_branch.eidos | examples/basics/30_curried_pattern_branch.eidos |
| examples/53_module_exports_and_reexports.eidos | examples/basics/53_module_exports_and_reexports.eidos |
| examples/62_option_suffix_coalesce.eidos | examples/basics/62_option_suffix_coalesce.eidos |
| examples/63_let_question_option_result.eidos | examples/basics/63_let_question_option_result.eidos |
| examples/66_default_ctor_sugar.eidos | examples/basics/66_default_ctor_sugar.eidos |
| examples/71_implicit_unit_selection.eidos | examples/basics/71_implicit_unit_selection.eidos |
| examples/09_effect_tag_call.eidos | examples/effects/09_effect_tag_call.eidos |
| examples/51_qualified_effect_paths.eidos | examples/effects/51_qualified_effect_paths.eidos |
| examples/52_nested_qualified_effect_paths.eidos | examples/effects/52_nested_qualified_effect_paths.eidos |
| examples/55_ffi_basic.eidos | examples/ffi/55_ffi_basic.eidos |
| examples/56_ffi_pointer_ops.eidos | examples/ffi/56_ffi_pointer_ops.eidos |
| examples/57_ffi_callback.eidos | examples/ffi/57_ffi_callback.eidos |
| examples/58_ffi_qsort.eidos | examples/ffi/58_ffi_qsort.eidos |
| examples/43_open_alias_trait_impl.eidos | examples/functional/43_open_alias_trait_impl.eidos |
| examples/44_std_traversable_alias_applicative.eidos | examples/functional/44_std_traversable_alias_applicative.eidos |
| examples/45_std_list_traversable_alias_applicative.eidos | examples/functional/45_std_list_traversable_alias_applicative.eidos |
| examples/46_traversable_alias_applicative_empty_cases.eidos | examples/functional/46_traversable_alias_applicative_empty_cases.eidos |
| examples/47_traversable_sequence_alias_applicative.eidos | examples/functional/47_traversable_sequence_alias_applicative.eidos |
| examples/48_sequence_result_applicative.eidos | examples/functional/48_sequence_result_applicative.eidos |
| examples/49_generic_traversable_sequence.eidos | examples/functional/49_generic_traversable_sequence.eidos |
| examples/55_functional_infix_chain_style.eidos | examples/functional/55_functional_infix_chain_style.eidos |
| examples/31_hkt_parenthesized_kind.eidos | examples/generics/31_hkt_parenthesized_kind.eidos |
| examples/32_hkt_adt_inferred_kind.eidos | examples/generics/32_hkt_adt_inferred_kind.eidos |
| examples/33_hkt_effect_polymorphism.eidos | examples/generics/33_hkt_effect_polymorphism.eidos |
| examples/34_hkt_trait_inferred_kind.eidos | examples/generics/34_hkt_trait_inferred_kind.eidos |
| examples/35_hkt_trait_constraint_type_args.eidos | examples/generics/35_hkt_trait_constraint_type_args.eidos |
| examples/36_hkt_trait_constraint_kind_mismatch.eidos | examples/generics/36_hkt_trait_constraint_kind_mismatch.eidos |
| examples/37_trait_impl_generic_trait_args.eidos | examples/generics/37_trait_impl_generic_trait_args.eidos |
| examples/68_const_generics.eidos | examples/generics/68_const_generics.eidos |
| examples/01_literals_bindings.eidos | examples/getting_started/01_literals_bindings.eidos |
| examples/64_constructor_constant_derive.eidos | examples/meta/64_constructor_constant_derive.eidos |
| examples/69_meta_reflection_derive.eidos | examples/meta/69_meta_reflection_derive.eidos |
| examples/38_borrow_effect_decoupling.eidos | examples/ownership/38_borrow_effect_decoupling.eidos |
| examples/39_unary_deref.eidos | examples/ownership/39_unary_deref.eidos |
| examples/40_unary_ref.eidos | examples/ownership/40_unary_ref.eidos |
| examples/41_adt_ref_fields.eidos | examples/ownership/41_adt_ref_fields.eidos |
| examples/54_return_borrow_param_alias.eidos | examples/ownership/54_return_borrow_param_alias.eidos |
| examples/10_match_guard.eidos | examples/pattern/10_match_guard.eidos |
| examples/11_advanced_patterns.eidos | examples/pattern/11_advanced_patterns.eidos |
| examples/13_if_let_pattern.eidos | examples/pattern/13_if_let_pattern.eidos |
| examples/14_while_let_pattern.eidos | examples/pattern/14_while_let_pattern.eidos |
| examples/15_pattern_binding_modes.eidos | examples/pattern/15_pattern_binding_modes.eidos |
| examples/16_list_rest_pattern.eidos | examples/pattern/16_list_rest_pattern.eidos |
| examples/17_list_guarded_coverage.eidos | examples/pattern/17_list_guarded_coverage.eidos |
| examples/18_view_guard_unsat.eidos | examples/pattern/18_view_guard_unsat.eidos |
| examples/19_view_guard_finite_set.eidos | examples/pattern/19_view_guard_finite_set.eidos |
| examples/20_guard_algebra_unsat.eidos | examples/pattern/20_guard_algebra_unsat.eidos |
| examples/21_view_guard_mixed_and_not_unsat.eidos | examples/pattern/21_view_guard_mixed_and_not_unsat.eidos |
| examples/22_nested_view_combinator_coverage.eidos | examples/pattern/22_nested_view_combinator_coverage.eidos |
| examples/23_view_guard_other_and_not_unsat.eidos | examples/pattern/23_view_guard_other_and_not_unsat.eidos |
| examples/24_view_guard_nested_as_other_unsat.eidos | examples/pattern/24_view_guard_nested_as_other_unsat.eidos |
| examples/25_view_guard_mixed_view_nonview_conservative.eidos | examples/pattern/25_view_guard_mixed_view_nonview_conservative.eidos |
| examples/26_view_guard_uncertain_only_conservative.eidos | examples/pattern/26_view_guard_uncertain_only_conservative.eidos |
| examples/27_pattern_guard_binding.eidos | examples/pattern/27_pattern_guard_binding.eidos |
| examples/70_decision_table.eidos | examples/pattern/70_decision_table.eidos |
| examples/29_precompiled_stdlib.eidos | examples/stdlib/29_precompiled_stdlib.eidos |
| examples/42_stdlib_safe_and_traits.eidos | examples/stdlib/42_stdlib_safe_and_traits.eidos |
| examples/65_game_math_vectors.eidos | examples/stdlib/65_game_math_vectors.eidos |
| examples/05_trait_impl_declaration.eidos | examples/traits/05_trait_impl_declaration.eidos |
| examples/50_qualified_trait_method_paths.eidos | examples/traits/50_qualified_trait_method_paths.eidos |
