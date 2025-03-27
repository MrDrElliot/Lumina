
#ifdef GET_DIAG_ARRAYS
static const int16_t DiagArrays[] = {
  /* Empty */ -1,
  /* DiagArray1 */ diag::warn_pragma_message, -1,
  /* DiagArray2 */ diag::pp_hash_warning, -1,
  /* DiagArray3 */ diag::warn_cfstring_truncated, -1,
  /* DiagArray5 */ diag::warn_independentclass_attribute, diag::warn_ptr_independentclass_attribute, -1,
  /* DiagArray6 */ diag::warn_nsobject_attribute, -1,
  /* DiagArray7 */ diag::warn_sme_locally_streaming_has_vl_args_returns, diag::warn_sme_streaming_pass_return_vl_to_non_streaming, -1,
  /* DiagArray9 */ diag::warn_abs_too_small, diag::warn_pointer_abs, diag::warn_unsigned_abs, diag::warn_wrong_absolute_value_type, -1,
  /* DiagArray10 */ diag::warn_abstract_final_class, -1,
  /* DiagArray11 */ diag::warn_abstract_vbase_init_ignored, -1,
  /* DiagArray13 */ diag::warn_taking_address_of_packed_member, -1,
  /* DiagArray14 */ diag::ext_typecheck_addrof_temporary, -1,
  /* DiagArray16 */ diag::warn_not_xl_compatible, diag::warn_pragma_align_not_xl_compatible, -1,
  /* DiagArray17 */ diag::ext_alias_template_in_declarative_nns, -1,
  /* DiagArray18 */ diag::warn_param_mismatched_alignment, -1,
  /* DiagArray20 */ diag::warn_alloca, -1,
  /* DiagArray21 */ diag::warn_alloca_align_alignof, -1,
  /* DiagArray22 */ diag::warn_always_inline_coroutine, -1,
  /* DiagArray23 */ diag::warn_ambiguous_suitable_delete_function_found, -1,
  /* DiagArray24 */ diag::warn_misplaced_ellipsis_vararg, -1,
  /* DiagArray25 */ diag::warn_pp_ambiguous_macro, -1,
  /* DiagArray26 */ diag::ext_nested_name_member_ref_lookup_ambiguous, -1,
  /* DiagArray27 */ diag::ext_ovl_ambiguous_oper_binary_reversed, -1,
  /* DiagArray28 */ diag::warn_incompatible_analyzer_plugin_api, -1,
  /* DiagArray29 */ diag::warn_android_unversioned_fallback, -1,
  /* DiagArray30 */ diag::warn_arith_conv_mixed_anon_enum_types, -1,
  /* DiagArray31 */ diag::ext_abstract_pack_declarator_parens, -1,
  /* DiagArray32 */ diag::warn_apinotes_message, -1,
  /* DiagArray34 */ diag::warn_arc_bridge_cast_nonarc, -1,
  /* DiagArray35 */ diag::warn_arc_possible_repeated_use_of_weak, -1,
  /* DiagArray36 */ diag::warn_arc_object_memaccess, -1,
  /* DiagArray37 */ diag::warn_arc_perform_selector_leaks, -1,
  /* DiagArray38 */ diag::warn_arc_repeated_use_of_weak, -1,
  /* DiagArray39 */ diag::warn_arc_retain_cycle, -1,
  /* DiagArray40 */ diag::warn_arc_literal_assign, diag::warn_arc_retained_assign, diag::warn_arc_retained_property_assign, -1,
  /* DiagArray41 */ diag::warn_argument_invalid_range, -1,
  /* DiagArray42 */ diag::warn_argument_undefined_behaviour, -1,
  /* DiagArray43 */ diag::warn_array_index_exceeds_bounds, diag::warn_array_index_exceeds_max_addressable_bounds, diag::warn_array_index_precedes_bounds, diag::warn_ptr_arith_exceeds_max_addressable_bounds, diag::warn_static_array_too_small, -1,
  /* DiagArray44 */ diag::warn_ptr_arith_exceeds_bounds, diag::warn_ptr_arith_precedes_bounds, -1,
  /* DiagArray45 */ diag::warn_inconsistent_array_form, -1,
  /* DiagArray47 */ diag::warn_asm_mismatched_size_modifier, -1,
  /* DiagArray48 */ diag::warn_not_in_enum_assignment, -1,
  /* DiagArray49 */ diag::warn_assume_side_effects, -1,
  /* DiagArray51 */ diag::warn_atimport_in_framework_header, -1,
  /* DiagArray52 */ diag::warn_atomic_member_access, -1,
  /* DiagArray53 */ diag::warn_atomic_op_misaligned, diag::warn_atomic_op_oversized, -1,
  /* DiagArray54 */ diag::warn_atomic_implicit_seq_cst, -1,
  /* DiagArray55 */ diag::warn_atomic_op_has_invalid_memory_order, -1,
  /* DiagArray57 */ diag::warn_atomic_property_rule, -1,
  /* DiagArray58 */ diag::warn_attribute_packed_for_bitfield, -1,
  /* DiagArray59 */ diag::warn_fe_backend_warning_attr, diag::warn_toc_unsupported_type, -1,
  /* DiagArray61 */ diag::ext_c23_auto_non_plain_identifier, -1,
  /* DiagArray62 */ diag::warn_drv_disabling_vptr_no_rtti_default, -1,
  /* DiagArray64 */ diag::ext_auto_storage_class, -1,
  /* DiagArray65 */ diag::warn_auto_var_is_id, -1,
  /* DiagArray66 */ diag::warn_availability_and_unavailable, diag::warn_availability_fuchsia_unavailable_minor, diag::warn_availability_on_static_initializer, diag::warn_availability_swift_unavailable_deprecated_only, diag::warn_availability_unknown_environment, diag::warn_availability_unknown_platform, diag::warn_availability_version_ordering, diag::warn_expected_consistent_version_separator, diag::warn_mismatched_availability, diag::warn_mismatched_availability_override, diag::warn_mismatched_availability_override_unavail, -1,
  /* DiagArray67 */ diag::warn_drv_avr_family_linking_stdlibs_not_implemented, diag::warn_drv_avr_libc_not_found, diag::warn_drv_avr_linker_section_addresses_not_implemented, diag::warn_drv_avr_mcu_not_specified, diag::warn_drv_avr_stdlib_not_linked, -1,
  /* DiagArray68 */ diag::warn_fe_backend_plugin, diag::warn_fe_backend_resource_limit, -1,
  /* DiagArray69 */ diag::backslash_newline_space, -1,
  /* DiagArray70 */ diag::warn_bad_function_cast, -1,
  /* DiagArray72 */ diag::ext_rvalue_to_reference_access_ctor, diag::ext_rvalue_to_reference_temp_copy_no_viable, -1,
  /* DiagArray73 */ diag::ext_decomp_decl_cond, -1,
  /* DiagArray74 */ diag::ext_bit_int, diag::ext_cxx_bitint_suffix, -1,
  /* DiagArray75 */ diag::warn_impcast_bitfield_precision_constant, -1,
  /* DiagArray76 */ diag::warn_bitfield_too_small_for_enum, diag::warn_signed_bitfield_enum_conversion, diag::warn_unsigned_bitfield_assigned_signed_enum, -1,
  /* DiagArray77 */ diag::warn_bitfield_width_exceeds_type_width, -1,
  /* DiagArray78 */ diag::warn_precedence_bitwise_conditional, -1,
  /* DiagArray79 */ diag::warn_bitwise_instead_of_logical, -1,
  /* DiagArray80 */ diag::warn_bitwise_op_in_bitwise_op, -1,
  /* DiagArray81 */ diag::warn_block_capture_autoreleasing, -1,
  /* DiagArray82 */ diag::warn_impcast_bool_to_null_pointer, -1,
  /* DiagArray84 */ diag::warn_bitwise_negation_bool, -1,
  /* DiagArray85 */ diag::warn_counted_by_attr_elt_type_unknown_size, -1,
  /* DiagArray86 */ diag::warn_braces_around_init, -1,
  /* DiagArray87 */ diag::warn_incompatible_branch_protection_option, diag::warn_target_unsupported_branch_protection_attribute, diag::warn_unsupported_branch_protection, diag::warn_unsupported_branch_protection_spec, -1,
  /* DiagArray88 */ diag::warn_objc_invalid_bridge, diag::warn_objc_invalid_bridge_to_cf, -1,
  /* DiagArray89 */ diag::warn_assume_aligned_too_great, -1,
  /* DiagArray90 */ diag::ext_pp_redef_builtin_macro, diag::ext_pp_undef_builtin_macro, -1,
  /* DiagArray91 */ diag::warn_builtin_chk_overflow, -1,
  /* DiagArray92 */ diag::warn_implicit_decl_requires_sysheader, -1,
  /* DiagArray93 */ diag::warn_zero_size_struct_union_compat, -1,
  /* DiagArray97 */ diag::ext_old_implicitly_unsigned_long_cxx, diag::warn_auto_storage_class, diag::warn_cxx11_compat_user_defined_literal, diag::warn_cxx11_keyword, diag::warn_cxx11_right_shift_in_template_arg, diag::warn_explicit_instantiation_inline_0x, diag::warn_explicit_instantiation_must_be_global_0x, diag::warn_explicit_instantiation_out_of_scope_0x, diag::warn_explicit_instantiation_unqualified_wrong_namespace_0x, diag::warn_old_implicitly_unsigned_long_cxx, -1,
  /* DiagArray98 */ diag::warn_deprecated_string_literal_conversion, -1,
  /* DiagArray100 */ diag::warn_cxx11_compat_reserved_user_defined_literal, -1,
  /* DiagArray101 */ diag::ext_alias_declaration, diag::ext_array_size_conversion, diag::ext_auto_type_specifier, diag::ext_cxx11_enum_fixed_underlying_type, diag::ext_defaulted_deleted_function, diag::ext_enumerator_list_comma_cxx, diag::ext_explicit_conversion_functions, diag::ext_extern_template, diag::ext_for_range, diag::ext_generalized_initializer_lists, diag::ext_lambda, diag::ext_nested_name_spec_is_enum, diag::ext_nonclass_type_friend, diag::ext_nonstatic_member_init, diag::ext_override_control_keyword, diag::ext_ref_qualifier, diag::ext_rvalue_reference, diag::ext_scoped_enum, diag::ext_static_data_member_in_union, diag::ext_template_arg_object_internal, diag::ext_template_outside_of_template, diag::ext_template_parameter_default_in_function_template, diag::ext_typename_outside_of_template, diag::ext_unelaborated_friend_type, diag::ext_variadic_templates, diag::warn_ext_cxx11_attributes, -1,
  /* DiagArray102 */ diag::ext_extra_semi_cxx11, -1,
  /* DiagArray103 */ diag::ext_inline_namespace, -1,
  /* DiagArray104 */ diag::ext_cxx11_longlong, -1,
  /* DiagArray105 */ diag::ext_cce_narrowing, diag::ext_init_list_constant_narrowing, diag::ext_init_list_type_narrowing, diag::ext_init_list_variable_narrowing, diag::warn_init_list_constant_narrowing, diag::warn_init_list_type_narrowing, diag::warn_init_list_variable_narrowing, -1,
  /* DiagArray106 */ diag::ext_init_list_constant_narrowing_const_reference, diag::ext_init_list_type_narrowing_const_reference, diag::ext_init_list_variable_narrowing_const_reference, -1,
  /* DiagArray107 */ diag::ext_cxx14_attr, -1,
  /* DiagArray108 */ diag::ext_binary_literal_cxx14, -1,
  /* DiagArray111 */ diag::ext_constexpr_body_invalid_stmt, diag::ext_constexpr_body_multiple_return, diag::ext_constexpr_local_var, diag::ext_constexpr_type_definition, diag::ext_decltype_auto_type_specifier, diag::ext_init_capture, diag::ext_variable_template, -1,
  /* DiagArray112 */ diag::ext_cxx17_attr, -1,
  /* DiagArray114 */ diag::warn_cxx17_compat_exception_spec_in_signature, -1,
  /* DiagArray116 */ diag::ext_auto_new_list_init, diag::ext_constexpr_if, diag::ext_constexpr_on_lambda_cxx17, diag::ext_cxx_static_assert_no_message, diag::ext_decomp_decl, diag::ext_fold_expression, diag::ext_for_range_begin_end_types_differ, diag::ext_hex_literal_invalid, diag::ext_init_statement, diag::ext_inline_variable, diag::ext_multi_using_declaration, diag::ext_nested_namespace_definition, diag::ext_ns_enum_attribute, diag::ext_star_this_lambda_capture_cxx17, diag::ext_template_template_param_typename, diag::ext_using_attribute_ns, diag::ext_using_declaration_pack, -1,
  /* DiagArray121 */ diag::ext_cxx20_attr, -1,
  /* DiagArray122 */ diag::warn_cxx17_compat_implicit_typename, diag::warn_cxx20_compat_aggregate_init_with_ctors, diag::warn_cxx20_compat_consteval, diag::warn_cxx20_compat_constinit, diag::warn_cxx20_compat_explicit_bool, diag::warn_cxx20_compat_spaceship, diag::warn_cxx20_compat_use_of_unaddressable_function, diag::warn_cxx20_compat_utf8_string, diag::warn_cxx20_keyword, -1,
  /* DiagArray124 */ diag::ext_cxx_designated_init, -1,
  /* DiagArray125 */ diag::ext_adl_only_template_id, diag::ext_bitfield_member_init, diag::ext_capture_binding, diag::ext_constexpr_body_invalid_stmt_cxx20, diag::ext_constexpr_ctor_missing_init, diag::ext_constexpr_function_try_block_cxx20, diag::ext_constexpr_local_var_no_init, diag::ext_constexpr_union_ctor_no_init, diag::ext_cxx_missing_varargs_arg, diag::ext_decomp_decl_spec, diag::ext_defaulted_comparison, diag::ext_equals_this_lambda_capture_cxx20, diag::ext_explicit_bool, diag::ext_for_range_init_stmt, diag::ext_implicit_typename, diag::ext_init_capture_pack, diag::ext_inline_nested_namespace_definition, diag::ext_lambda_template_parameter_list, diag::ext_pointer_to_const_ref_member_on_rvalue, diag::ext_using_decl_scoped_enumerator, diag::ext_using_enum_declaration, diag::warn_cxx17_compat_aggregate_init_paren_list, -1,
  /* DiagArray126 */ diag::ext_cxx23_attr, -1,
  /* DiagArray127 */ diag::err_static_lambda, diag::ext_alias_in_init_statement, diag::ext_consteval_if, diag::ext_constexpr_body_invalid_stmt_cxx23, diag::ext_constexpr_static_var, diag::ext_cxx23_pp_directive, diag::ext_cxx23_size_t_suffix, diag::ext_cxx_label_end_of_compound_statement, diag::ext_lambda_missing_parens, diag::ext_operator_overload_static, -1,
  /* DiagArray128 */ diag::ext_decl_attrs_on_lambda, -1,
  /* DiagArray129 */ diag::ext_decl_attrs_on_binding, diag::ext_delete_with_message, diag::ext_pack_indexing, diag::ext_placeholder_var_definition, -1,
  /* DiagArray140 */ diag::warn_cxx11_compat_binary_literal, -1,
  /* DiagArray142 */ diag::warn_cxx98_compat_alias_declaration, diag::warn_cxx98_compat_alignas, diag::warn_cxx98_compat_alignof, diag::warn_cxx98_compat_attribute, diag::warn_cxx98_compat_auto_type_specifier, diag::warn_cxx98_compat_constexpr, diag::warn_cxx98_compat_ctor_list_init, diag::warn_cxx98_compat_decltype, diag::warn_cxx98_compat_defaulted_deleted_function, diag::warn_cxx98_compat_delegating_ctor, diag::warn_cxx98_compat_empty_scalar_initializer, diag::warn_cxx98_compat_empty_sizeless_initializer, diag::warn_cxx98_compat_enum_fixed_underlying_type, diag::warn_cxx98_compat_enum_nested_name_spec, diag::warn_cxx98_compat_explicit_conversion_functions, diag::warn_cxx98_compat_for_range, diag::warn_cxx98_compat_friend_is_member, diag::warn_cxx98_compat_generalized_initializer_lists, diag::warn_cxx98_compat_goto_into_protected_scope, diag::warn_cxx98_compat_indirect_goto_in_protected_scope, diag::warn_cxx98_compat_initializer_list_init, diag::warn_cxx98_compat_inline_namespace, diag::warn_cxx98_compat_lambda, diag::warn_cxx98_compat_less_colon_colon, diag::warn_cxx98_compat_literal_operator, diag::warn_cxx98_compat_literal_ucn_control_character, diag::warn_cxx98_compat_literal_ucn_escape_basic_scs, diag::warn_cxx98_compat_noexcept_decl, diag::warn_cxx98_compat_noexcept_expr, diag::warn_cxx98_compat_non_static_member_use, diag::warn_cxx98_compat_nonclass_type_friend, diag::warn_cxx98_compat_nonstatic_member_init, diag::warn_cxx98_compat_nontrivial_union_or_anon_struct_member, diag::warn_cxx98_compat_nullptr, diag::warn_cxx98_compat_override_control_keyword, diag::warn_cxx98_compat_pass_non_pod_arg_to_vararg, diag::warn_cxx98_compat_raw_string_literal, diag::warn_cxx98_compat_ref_qualifier, diag::warn_cxx98_compat_reference_list_init, diag::warn_cxx98_compat_rvalue_reference, diag::warn_cxx98_compat_scoped_enum, diag::warn_cxx98_compat_sfinae_access_control, diag::warn_cxx98_compat_static_assert, diag::warn_cxx98_compat_static_data_member_in_union, diag::warn_cxx98_compat_switch_into_protected_scope, diag::warn_cxx98_compat_template_arg_extra_parens, diag::warn_cxx98_compat_template_arg_null, diag::warn_cxx98_compat_template_arg_object_internal, diag::warn_cxx98_compat_template_outside_of_template, diag::warn_cxx98_compat_template_parameter_default_in_function_template, diag::warn_cxx98_compat_trailing_return_type, diag::warn_cxx98_compat_two_right_angle_brackets, diag::warn_cxx98_compat_typename_outside_of_template, diag::warn_cxx98_compat_unelaborated_friend_type, diag::warn_cxx98_compat_unicode_literal, diag::warn_cxx98_compat_unicode_type, diag::warn_cxx98_compat_using_decl_constructor, diag::warn_cxx98_compat_variadic_templates, -1,
  /* DiagArray143 */ diag::warn_cxx98_compat_temp_copy, -1,
  /* DiagArray144 */ diag::warn_cxx98_compat_top_level_semi, -1,
  /* DiagArray145 */ diag::warn_cxx98_compat_template_arg_local_type, -1,
  /* DiagArray146 */ diag::warn_cxx98_compat_array_size_conversion, diag::warn_cxx98_compat_cast_fn_obj, diag::warn_cxx98_compat_empty_fnmacro_arg, diag::warn_cxx98_compat_enumerator_list_comma, diag::warn_cxx98_compat_extern_template, diag::warn_cxx98_compat_longlong, diag::warn_cxx98_compat_no_newline_eof, diag::warn_cxx98_compat_pp_line_too_big, diag::warn_cxx98_compat_variadic_macro, -1,
  /* DiagArray147 */ diag::warn_cxx98_compat_template_arg_unnamed_type, -1,
  /* DiagArray148 */ diag::ext_anonymous_union, diag::ext_c11_anonymous_struct, diag::ext_c11_feature, diag::ext_typecheck_compare_complete_incomplete_pointers, -1,
  /* DiagArray149 */ diag::warn_c23_keyword, -1,
  /* DiagArray150 */ diag::ext_binary_literal, diag::ext_c23_bitint_suffix, diag::ext_c23_pp_directive, diag::ext_c_empty_initializer, diag::ext_c_label_end_of_compound_statement, diag::ext_c_label_followed_by_declaration, diag::ext_c_missing_varargs_arg, diag::ext_c_nullptr, diag::ext_c_static_assert_no_message, diag::ext_parameter_name_omitted_c23, diag::ext_type_defined_in_offsetof, diag::warn_ext_c23_attributes, -1,
  /* DiagArray153 */ diag::warn_c99_compat_unicode_id, diag::warn_c99_compat_unicode_literal, diag::warn_c99_keyword, diag::warn_old_implicitly_unsigned_long, -1,
  /* DiagArray154 */ diag::ext_designated_init, diag::ext_designated_init_array, diag::ext_designated_init_brace_elision, diag::ext_designated_init_mixed, diag::ext_designated_init_nested, -1,
  /* DiagArray155 */ diag::ext_aggregate_init_not_constant, diag::ext_c99_array_usage, diag::ext_c99_compound_literal, diag::ext_c99_feature, diag::ext_c99_flexible_array_member, diag::ext_c99_variable_decl_in_for_loop, diag::ext_c99_whitespace_required_after_macro_name, diag::ext_empty_fnmacro_arg, diag::ext_enumerator_list_comma_c, diag::ext_hex_constant_invalid, -1,
  /* DiagArray156 */ diag::warn_call_to_pure_virtual_member_function_from_ctor_dtor, -1,
  /* DiagArray157 */ diag::warn_called_once_gets_called_twice, diag::warn_called_once_never_called, diag::warn_called_once_never_called_when, -1,
  /* DiagArray158 */ diag::warn_cast_align, -1,
  /* DiagArray159 */ diag::warn_cast_calling_conv, -1,
  /* DiagArray161 */ diag::warn_cast_function_type, -1,
  /* DiagArray162 */ diag::warn_cast_function_type_strict, -1,
  /* DiagArray163 */ diag::warn_cast_pointer_from_sel, -1,
  /* DiagArray164 */ diag::warn_cast_qual, diag::warn_cast_qual2, -1,
  /* DiagArray165 */ diag::ext_bad_cxx_cast_qualifiers_away_incoherent, -1,
  /* DiagArray167 */ diag::warn_omp_section_is_char, diag::warn_subscript_is_char, -1,
  /* DiagArray168 */ diag::warn_drv_yc_multiple_inputs_clang_cl, diag::warn_drv_ycyu_different_arg_clang_cl, diag::warn_pp_hdrstop_filename_ignored, diag::warn_pp_macro_def_mismatch_with_pch, -1,
  /* DiagArray169 */ diag::warn_conv_to_base_not_used, diag::warn_conv_to_self_not_used, diag::warn_conv_to_void_not_used, -1,
  /* DiagArray170 */ diag::warn_pass_class_arg_to_vararg, -1,
  /* DiagArray171 */ diag::warn_cmse_nonsecure_union, -1,
  /* DiagArray172 */ diag::warn_comma_operator, -1,
  /* DiagArray173 */ diag::escaped_newline_block_comment_end, diag::ext_line_comment, diag::ext_multi_line_line_comment, diag::warn_nested_block_comment, -1,
  /* DiagArray175 */ diag::ext_typecheck_comparison_of_distinct_pointers, -1,
  /* DiagArray176 */ diag::warn_completion_handler_called_twice, diag::warn_completion_handler_never_called, diag::warn_completion_handler_never_called_when, -1,
  /* DiagArray177 */ diag::ext_complex_component_init, -1,
  /* DiagArray179 */ diag::warn_compound_token_split_by_macro, -1,
  /* DiagArray180 */ diag::warn_compound_token_split_by_whitespace, -1,
  /* DiagArray181 */ diag::ext_typecheck_cond_pointer_integer_mismatch, -1,
  /* DiagArray182 */ diag::warn_maybe_uninit_var, -1,
  /* DiagArray183 */ diag::warn_module_config_macro_undef, -1,
  /* DiagArray184 */ diag::warn_impcast_integer_precision_constant, -1,
  /* DiagArray185 */ diag::warn_is_constant_evaluated_always_true_constexpr, -1,
  /* DiagArray186 */ diag::warn_logical_instead_of_bitwise, -1,
  /* DiagArray187 */ diag::warn_cxx14_compat_constexpr_not_const, -1,
  /* DiagArray188 */ diag::warn_attr_on_unconsumable_class, diag::warn_loop_state_mismatch, diag::warn_param_return_typestate_mismatch, diag::warn_param_typestate_mismatch, diag::warn_return_typestate_for_unconsumable_type, diag::warn_return_typestate_mismatch, diag::warn_use_in_invalid_state, diag::warn_use_of_temp_in_invalid_state, -1,
  /* DiagArray189 */ diag::warn_hlsl_impcast_vector_truncation, diag::warn_impcast_complex_scalar, diag::warn_impcast_vector_scalar, diag::warn_opencl_generic_address_space_arg, diag::warn_template_arg_negative, diag::warn_template_arg_too_large, -1,
  /* DiagArray191 */ diag::warn_non_aligned_allocation_function, -1,
  /* DiagArray192 */ diag::warn_coroutine_handle_address_invalid_return_type, -1,
  /* DiagArray193 */ diag::warn_coroutine_promise_unhandled_exception_required_with_exceptions, -1,
  /* DiagArray194 */ diag::warn_unreachable_default, -1,
  /* DiagArray196 */ diag::warn_objc_cdirective_format_string, -1,
  /* DiagArray197 */ diag::warn_ctad_maybe_unsupported, -1,
  /* DiagArray199 */ diag::warn_ctu_incompat_triple, -1,
  /* DiagArray200 */ diag::warn_attribute_argument_n_negative, diag::warn_cuda_attr_lambda_position, diag::warn_kern_is_inline, diag::warn_kern_is_method, diag::warn_pragma_unroll_cuda_value_in_parens, -1,
  /* DiagArray201 */ diag::warn_default_atomic_custom_getter_setter, -1,
  /* DiagArray202 */ diag::ext_cxx11_attr_placement, -1,
  /* DiagArray203 */ diag::warn_dangling_variable, -1,
  /* DiagArray204 */ diag::warn_dangling_else, -1,
  /* DiagArray205 */ diag::warn_bind_ref_member_to_parameter, diag::warn_dangling_member, diag::warn_init_ptr_member_to_parameter_addr, diag::warn_new_dangling_reference, -1,
  /* DiagArray206 */ diag::warn_dangling_lifetime_pointer, diag::warn_dangling_lifetime_pointer_member, -1,
  /* DiagArray207 */ diag::warn_new_dangling_initializer_list, -1,
  /* DiagArray208 */ diag::warn_drv_darwin_sdk_invalid_settings, -1,
  /* DiagArray209 */ diag::warn_pp_date_time, -1,
  /* DiagArray210 */ diag::warn_dealloc_in_category, -1,
  /* DiagArray211 */ diag::warn_debug_compression_unavailable, -1,
  /* DiagArray212 */ diag::ext_mixed_decls_code, diag::warn_mixed_decls_code, -1,
  /* DiagArray213 */ diag::warn_defaulted_comparison_deleted, diag::warn_defaulted_method_deleted, -1,
  /* DiagArray214 */ diag::warn_drv_delayed_template_parsing_after_cxx20, -1,
  /* DiagArray215 */ diag::warn_delegating_ctor_cycle, -1,
  /* DiagArray216 */ diag::warn_delete_abstract_non_virtual_dtor, -1,
  /* DiagArray217 */ diag::ext_delete_void_ptr_operand, diag::warn_delete_incomplete, -1,
  /* DiagArray218 */ diag::warn_delete_non_virtual_dtor, -1,
  /* DiagArray220 */ diag::ext_delimited_escape_sequence, -1,
  /* DiagArray221 */ diag::warn_deprecated_lax_vec_conv_all, -1,
  /* DiagArray222 */ diag::warn_O4_is_O3, diag::warn_access_decl_deprecated, diag::warn_drv_deprecated_arg, diag::warn_drv_deprecated_custom, diag::warn_drv_treating_input_as_cxx, diag::warn_omp_depend_in_ordered_deprecated, diag::warn_omp_minus_in_reduction_deprecated, diag::warn_option_invalid_ocl_version, diag::warn_vector_long_decl_spec_combination, -1,
  /* DiagArray223 */ diag::warn_deprecated_altivec_src_compat, -1,
  /* DiagArray224 */ diag::warn_arith_conv_mixed_anon_enum_types_cxx20, -1,
  /* DiagArray225 */ diag::warn_depr_array_comparison, -1,
  /* DiagArray226 */ diag::warn_deprecated_noreturn_spelling, diag::warn_type_attribute_deprecated_on_decl, diag::warn_vector_mode_deprecated, -1,
  /* DiagArray227 */ diag::warn_deprecated_builtin, -1,
  /* DiagArray228 */ diag::warn_deprecated_comma_subscript, -1,
  /* DiagArray229 */ diag::warn_deprecated_copy, -1,
  /* DiagArray231 */ diag::warn_deprecated_copy_with_dtor, -1,
  /* DiagArray232 */ diag::warn_deprecated_copy_with_user_provided_copy, -1,
  /* DiagArray233 */ diag::warn_deprecated_copy_with_user_provided_dtor, -1,
  /* DiagArray234 */ diag::warn_deprecated_for_co_await, -1,
  /* DiagArray235 */ diag::warn_atl_uuid_deprecated, diag::warn_cstyle_param, diag::warn_deprecated, diag::warn_deprecated_fwdclass_message, diag::warn_deprecated_message, diag::warn_property_method_deprecated, -1,
  /* DiagArray236 */ diag::warn_exception_spec_deprecated, -1,
  /* DiagArray237 */ diag::warn_comparison_mixed_enum_types_cxx20, -1,
  /* DiagArray238 */ diag::warn_conditional_mixed_enum_types_cxx20, -1,
  /* DiagArray239 */ diag::warn_arith_conv_mixed_enum_types_cxx20, -1,
  /* DiagArray240 */ diag::warn_arith_conv_enum_float_cxx20, -1,
  /* DiagArray241 */ diag::warn_deprecated_def, diag::warn_unavailable_def, -1,
  /* DiagArray242 */ diag::warn_increment_bool, -1,
  /* DiagArray243 */ diag::warn_deprecated_literal_operator_id, -1,
  /* DiagArray244 */ diag::warn_deprecated_module_dot_map, -1,
  /* DiagArray245 */ diag::warn_drv_deprecated_arg_no_relaxed_template_template_args, -1,
  /* DiagArray246 */ diag::warn_non_prototype_changes_behavior, diag::warn_strict_uses_without_prototype, -1,
  /* DiagArray247 */ diag::warn_objc_isa_assign, diag::warn_objc_isa_use, -1,
  /* DiagArray248 */ diag::warn_objc_pointer_masking, -1,
  /* DiagArray249 */ diag::warn_objc_pointer_masking_performSelector, -1,
  /* DiagArray250 */ diag::warn_pragma_deprecated_macro_use, -1,
  /* DiagArray251 */ diag::warn_deprecated_redundant_constexpr_static_def, -1,
  /* DiagArray252 */ diag::warn_deprecated_register, -1,
  /* DiagArray253 */ diag::warn_deprecated_this_capture, -1,
  /* DiagArray254 */ diag::warn_ext_int_deprecated, -1,
  /* DiagArray255 */ diag::warn_deprecated_increment_decrement_volatile, diag::warn_deprecated_simple_assign_volatile, diag::warn_deprecated_volatile_param, diag::warn_deprecated_volatile_return, diag::warn_deprecated_volatile_structured_binding, -1,
  /* DiagArray257 */ diag::warn_direct_ivar_access, -1,
  /* DiagArray258 */ diag::pp_disabled_macro_expansion, -1,
  /* DiagArray261 */ diag::warn_conflicting_param_modifiers, diag::warn_conflicting_ret_type_modifiers, -1,
  /* DiagArray263 */ diag::warn_remainder_division_by_zero, -1,
  /* DiagArray264 */ diag::warn_attribute_dll_redeclaration, -1,
  /* DiagArray265 */ diag::warn_attribute_dllexport_explicit_instantiation_decl, -1,
  /* DiagArray266 */ diag::warn_attribute_dllimport_static_field_definition, -1,
  /* DiagArray267 */ diag::warn_doc_api_container_decl_mismatch, diag::warn_doc_block_command_duplicate, diag::warn_doc_block_command_empty_paragraph, diag::warn_doc_container_decl_mismatch, diag::warn_doc_function_method_decl_mismatch, diag::warn_doc_html_start_tag_expected_ident_or_greater, diag::warn_doc_html_start_tag_expected_quoted_string, diag::warn_doc_inline_command_not_enough_arguments, diag::warn_doc_param_duplicate, diag::warn_doc_param_invalid_direction, diag::warn_doc_param_not_attached_to_a_function_decl, diag::warn_doc_param_not_found, diag::warn_doc_returns_attached_to_a_void_function, diag::warn_doc_returns_not_attached_to_a_function_decl, diag::warn_doc_tparam_duplicate, diag::warn_doc_tparam_not_attached_to_a_template_decl, diag::warn_doc_tparam_not_found, diag::warn_not_a_doxygen_trailing_member_comment, diag::warn_splice_in_doxygen_comment, diag::warn_verbatim_block_end_without_start, -1,
  /* DiagArray268 */ diag::warn_doc_deprecated_not_sync, -1,
  /* DiagArray269 */ diag::warn_doc_html_end_forbidden, diag::warn_doc_html_end_unbalanced, diag::warn_doc_html_missing_end_tag, diag::warn_doc_html_start_end_mismatch, -1,
  /* DiagArray270 */ diag::warn_doc_param_spaces_in_direction, -1,
  /* DiagArray271 */ diag::warn_correct_comment_command_name, diag::warn_unknown_comment_command_name, -1,
  /* DiagArray272 */ diag::ext_dollar_in_identifier, -1,
  /* DiagArray273 */ diag::warn_impcast_double_promotion, -1,
  /* DiagArray274 */ diag::ext_dtor_name_ambiguous, diag::ext_dtor_named_in_wrong_scope, diag::ext_qualified_dtor_named_in_lexical_scope, -1,
  /* DiagArray275 */ diag::ext_destructor_typedef_name, -1,
  /* DiagArray276 */ diag::ext_duplicate_declspec, diag::ext_warn_duplicate_declspec, diag::warn_attribute_address_multiple_identical_qualifiers, diag::warn_duplicate_declspec, -1,
  /* DiagArray277 */ diag::warn_duplicate_enum_values, -1,
  /* DiagArray278 */ diag::warn_method_param_declaration, -1,
  /* DiagArray279 */ diag::warn_duplicate_method_decl, -1,
  /* DiagArray280 */ diag::warn_duplicate_protocol_def, -1,
  /* DiagArray281 */ diag::warn_drv_dxc_missing_dxv, -1,
  /* DiagArray282 */ diag::warn_dyn_class_memaccess, -1,
  /* DiagArray283 */ diag::ext_dynamic_exception_spec, -1,
  /* DiagArray284 */ diag::warn_eagerly_load_for_standard_cplusplus_modules, -1,
  /* DiagArray286 */ diag::ext_enum_base_in_type_specifier, -1,
  /* DiagArray287 */ diag::ext_elaborated_enum_class, -1,
  /* DiagArray288 */ diag::ext_embedded_directive, -1,
  /* DiagArray289 */ diag::warn_empty_for_body, diag::warn_empty_if_body, diag::warn_empty_range_based_for_body, diag::warn_empty_switch_body, diag::warn_empty_while_body, -1,
  /* DiagArray290 */ diag::ext_decomp_decl_empty, -1,
  /* DiagArray291 */ diag::warn_empty_init_statement, -1,
  /* DiagArray292 */ diag::ext_empty_translation_unit, -1,
  /* DiagArray293 */ diag::warn_incomplete_encoded_type, -1,
  /* DiagArray295 */ diag::warn_comparison_mixed_enum_types, -1,
  /* DiagArray296 */ diag::warn_conditional_mixed_enum_types, -1,
  /* DiagArray297 */ diag::warn_comparison_of_mixed_enum_types_switch, -1,
  /* DiagArray298 */ diag::warn_constexpr_unscoped_enum_out_of_range, -1,
  /* DiagArray299 */ diag::warn_impcast_different_enum_types, -1,
  /* DiagArray300 */ diag::warn_arith_conv_mixed_enum_types, -1,
  /* DiagArray301 */ diag::warn_arith_conv_enum_float, -1,
  /* DiagArray302 */ diag::ext_enum_too_large, diag::ext_enumerator_increment_too_large, -1,
  /* DiagArray303 */ diag::warn_cdtor_function_try_handler_mem_expr, diag::warn_exception_caught_by_earlier_handler, diag::warn_throw_in_noexcept_func, -1,
  /* DiagArray304 */ diag::ext_excess_initializers, diag::ext_excess_initializers_for_sizeless_type, diag::ext_excess_initializers_in_char_array_initializer, diag::ext_initializer_string_for_char_array_too_long, -1,
  /* DiagArray305 */ diag::warn_anyx86_excessive_regsave, -1,
  /* DiagArray306 */ diag::warn_exit_time_destructor, -1,
  /* DiagArray307 */ diag::warn_defined_in_function_type_macro, diag::warn_defined_in_object_type_macro, -1,
  /* DiagArray308 */ diag::warn_experimental_header_unit, -1,
  /* DiagArray309 */ diag::warn_direct_initialize_call, diag::warn_direct_super_initialize_call, -1,
  /* DiagArray310 */ diag::warn_arc_strong_pointer_objc_pointer, -1,
  /* DiagArray312 */ diag::warn_zero_size_struct_union_in_extern_c, -1,
  /* DiagArray313 */ diag::warn_extern_init, -1,
  /* DiagArray314 */ diag::warn_arm_interrupt_calling_convention, -1,
  /* DiagArray315 */ diag::warn_namespace_member_extra_qualification, -1,
  /* DiagArray316 */ diag::ext_extra_semi, diag::warn_extra_semi_after_mem_fn_def, -1,
  /* DiagArray317 */ diag::warn_null_statement, -1,
  /* DiagArray318 */ diag::ext_pp_extra_tokens_at_eol, diag::warn_omp_extra_tokens_at_eol, -1,
  /* DiagArray319 */ diag::warn_missing_symbol_graph_dir, -1,
  /* DiagArray320 */ diag::warn_final_dtor_non_final_class, -1,
  /* DiagArray321 */ diag::warn_pragma_final_macro, -1,
  /* DiagArray322 */ diag::ext_clang_c_enum_fixed_underlying_type, -1,
  /* DiagArray323 */ diag::warn_fixedpoint_constant_overflow, -1,
  /* DiagArray324 */ diag::warn_flag_enum_constant_out_of_range, -1,
  /* DiagArray325 */ diag::ext_flexible_array_in_array, diag::ext_flexible_array_in_struct, -1,
  /* DiagArray326 */ diag::warn_impcast_float_integer, -1,
  /* DiagArray327 */ diag::warn_floatingpoint_eq, -1,
  /* DiagArray328 */ diag::warn_impcast_float_to_integer, diag::warn_impcast_float_to_integer_out_of_range, -1,
  /* DiagArray329 */ diag::warn_impcast_float_to_integer_zero, -1,
  /* DiagArray330 */ diag::warn_redundant_loop_iteration, diag::warn_variables_not_in_loop_body, -1,
  /* DiagArray331 */ diag::warn_format_P_no_precision, diag::warn_format_P_with_objc_pointer, diag::warn_format_argument_needs_cast, diag::warn_format_bool_as_character, diag::warn_format_conversion_argument_type_mismatch, diag::warn_format_invalid_annotation, diag::warn_format_invalid_positional_specifier, diag::warn_format_mix_positional_nonpositional_args, diag::warn_format_nonsensical_length, diag::warn_format_string_is_wide_literal, diag::warn_format_zero_positional_specifier, diag::warn_missing_format_string, diag::warn_printf_ObjCflags_without_ObjCConversion, diag::warn_printf_asterisk_missing_arg, diag::warn_printf_asterisk_wrong_type, diag::warn_printf_empty_objc_flag, diag::warn_printf_format_string_contains_null_char, diag::warn_printf_format_string_not_null_terminated, diag::warn_printf_ignored_flag, diag::warn_printf_incomplete_specifier, diag::warn_printf_invalid_objc_flag, diag::warn_printf_narg_not_supported, diag::warn_printf_nonsensical_flag, diag::warn_printf_nonsensical_optional_amount, diag::warn_printf_positional_arg_exceeds_data_args, diag::warn_scanf_nonzero_width, diag::warn_scanf_scanlist_incomplete, -1,
  /* DiagArray332 */ diag::warn_printf_data_arg_not_used, -1,
  /* DiagArray333 */ diag::warn_printf_insufficient_data_args, -1,
  /* DiagArray334 */ diag::warn_format_invalid_conversion, -1,
  /* DiagArray335 */ diag::warn_format_non_standard, diag::warn_format_non_standard_conversion_spec, diag::warn_format_non_standard_positional_arg, -1,
  /* DiagArray336 */ diag::warn_format_nonliteral, -1,
  /* DiagArray337 */ diag::warn_format_overflow, -1,
  /* DiagArray338 */ diag::warn_format_overflow_non_kprintf, -1,
  /* DiagArray339 */ diag::warn_format_argument_needs_cast_pedantic, diag::warn_format_conversion_argument_type_mismatch_pedantic, -1,
  /* DiagArray340 */ diag::warn_format_nonliteral_noargs, -1,
  /* DiagArray341 */ diag::warn_format_conversion_argument_type_mismatch_signedness, -1,
  /* DiagArray342 */ diag::warn_format_truncation, -1,
  /* DiagArray343 */ diag::warn_format_truncation_non_kprintf, -1,
  /* DiagArray344 */ diag::warn_format_conversion_argument_type_mismatch_confusion, -1,
  /* DiagArray346 */ diag::warn_empty_format_string, -1,
  /* DiagArray348 */ diag::warn_fortify_scanf_overflow, diag::warn_fortify_source_overflow, diag::warn_fortify_source_size_mismatch, diag::warn_fortify_strlen_overflow, -1,
  /* DiagArray349 */ diag::warn_four_char_character_literal, -1,
  /* DiagArray350 */ diag::warn_frame_address, -1,
  /* DiagArray351 */ diag::warn_fe_backend_frame_larger_than, diag::warn_fe_frame_larger_than, -1,
  /* DiagArray353 */ diag::warn_framework_include_private_from_public, -1,
  /* DiagArray354 */ diag::warn_free_nonheap_object, -1,
  /* DiagArray355 */ diag::ext_enum_friend, -1,
  /* DiagArray356 */ diag::warn_function_def_in_objc_container, -1,
  /* DiagArray357 */ diag::warn_dispatch_body_ignored, diag::warn_multiversion_duplicate_entries, diag::warn_target_clone_duplicate_options, diag::warn_target_clone_no_impact_options, -1,
  /* DiagArray358 */ diag::warn_drv_fuse_ld_path, -1,
  /* DiagArray361 */ diag::ext_clang_diagnose_if, diag::ext_clang_enable_if, diag::ext_warn_gnu_final, diag::warn_attribute_on_function_definition, diag::warn_break_binds_to_switch, diag::warn_cleanup_ext, diag::warn_gcc_attribute_location, diag::warn_gcc_ignores_type_attr, diag::warn_gcc_requires_variadic_function, diag::warn_gcc_variable_decl_in_for_loop, diag::warn_loop_ctrl_binds_to_inner, -1,
  /* DiagArray362 */ diag::ext_generic_with_type_arg, -1,
  /* DiagArray363 */ diag::warn_global_constructor, diag::warn_global_destructor, -1,
  /* DiagArray364 */ diag::warn_drv_global_isel_incomplete, diag::warn_drv_global_isel_incomplete_opt, -1,
  /* DiagArray366 */ diag::ext_alignof_expr, -1,
  /* DiagArray367 */ diag::ext_gnu_anonymous_struct, -1,
  /* DiagArray368 */ diag::ext_array_init_parens, -1,
  /* DiagArray369 */ diag::ext_auto_type, -1,
  /* DiagArray371 */ diag::ext_gnu_case_range, -1,
  /* DiagArray372 */ diag::ext_integer_complex, -1,
  /* DiagArray373 */ diag::ext_array_init_copy, -1,
  /* DiagArray374 */ diag::ext_gnu_conditional_expr, -1,
  /* DiagArray375 */ diag::ext_gnu_array_range, diag::ext_gnu_missing_equal_designator, diag::ext_gnu_old_style_field_designator, -1,
  /* DiagArray377 */ diag::ext_empty_struct_union, diag::ext_flexible_array_empty_aggregate_gnu, diag::ext_no_named_members_in_struct_union, -1,
  /* DiagArray378 */ diag::ext_flexible_array_init, -1,
  /* DiagArray379 */ diag::ext_flexible_array_union_gnu, -1,
  /* DiagArray380 */ diag::ext_expr_not_ice, diag::ext_in_class_initializer_non_constant, diag::ext_vla_folded_to_constant, -1,
  /* DiagArray381 */ diag::ext_imaginary_constant, -1,
  /* DiagArray382 */ diag::ext_pp_include_next_directive, -1,
  /* DiagArray383 */ diag::warn_gnu_inline_cplusplus_without_extern, -1,
  /* DiagArray384 */ diag::ext_gnu_address_of_label, diag::ext_gnu_indirect_goto, -1,
  /* DiagArray385 */ diag::ext_pp_gnu_line_directive, -1,
  /* DiagArray386 */ diag::warn_gnu_null_ptr_arith, -1,
  /* DiagArray388 */ diag::ext_gnu_ptr_func_arith, diag::ext_gnu_subscript_void_type, diag::ext_gnu_void_ptr, -1,
  /* DiagArray389 */ diag::ext_forward_ref_enum_def, -1,
  /* DiagArray390 */ diag::ext_gnu_statement_expr, -1,
  /* DiagArray391 */ diag::ext_gnu_statement_expr_macro, -1,
  /* DiagArray392 */ diag::ext_in_class_initializer_float_type, -1,
  /* DiagArray393 */ diag::ext_string_literal_operator_template, -1,
  /* DiagArray394 */ diag::ext_typecheck_cast_to_union, -1,
  /* DiagArray395 */ diag::ext_variable_sized_type_in_struct, -1,
  /* DiagArray396 */ diag::ext_pp_line_zero, -1,
  /* DiagArray397 */ diag::ext_paste_comma, -1,
  /* DiagArray398 */ diag::warn_maybe_capture_bad_target_this_ptr, -1,
  /* DiagArray399 */ diag::warn_header_guard, -1,
  /* DiagArray400 */ diag::warn_using_directive_in_header, -1,
  /* DiagArray401 */ diag::warn_next_larger_fp_type_same_size_than_fp, -1,
  /* DiagArray402 */ diag::warn_hip_omp_target_directives, -1,
  /* DiagArray403 */ diag::warn_ignored_hip_only_option, -1,
  /* DiagArray404 */ diag::ext_hlsl_access_specifiers, -1,
  /* DiagArray405 */ diag::warn_condition_is_idiomatic_assignment, -1,
  /* DiagArray406 */ diag::ext_cannot_use_trivial_abi, diag::warn_alias_to_weak_alias, diag::warn_alias_with_section, diag::warn_aligned_attr_underaligned, diag::warn_attr_abi_tag_namespace, diag::warn_attribute_after_definition_ignored, diag::warn_attribute_cmse_entry_static, diag::warn_attribute_dllexport_explicit_instantiation_def, diag::warn_attribute_exclude_from_explicit_instantiation_local_class, diag::warn_attribute_has_no_effect_on_compile_time_if, diag::warn_attribute_has_no_effect_on_infinite_loop, diag::warn_attribute_iboutlet, diag::warn_attribute_ignored, diag::warn_attribute_ignored_for_field_of_type, diag::warn_attribute_ignored_no_calls_in_stmt, diag::warn_attribute_ignored_non_function_pointer, diag::warn_attribute_ignored_on_inline, diag::warn_attribute_ignored_on_non_definition, diag::warn_attribute_invalid_on_definition, diag::warn_attribute_no_decl, diag::warn_attribute_nonnull_no_pointers, diag::warn_attribute_nonnull_parm_no_args, diag::warn_attribute_not_on_decl, diag::warn_attribute_pointer_or_reference_only, diag::warn_attribute_pointers_only, diag::warn_attribute_precede_definition, diag::warn_attribute_return_pointers_only, diag::warn_attribute_return_pointers_refs_only, diag::warn_attribute_sentinel_named_arguments, diag::warn_attribute_sentinel_not_variadic, diag::warn_attribute_type_not_supported, diag::warn_attribute_type_not_supported_global, diag::warn_attribute_unknown_visibility, diag::warn_attribute_void_function_method, diag::warn_attribute_weak_on_field, diag::warn_attribute_weak_on_local, diag::warn_attribute_wrong_decl_type, diag::warn_attribute_wrong_decl_type_str, diag::warn_attributes_likelihood_ifstmt_conflict, diag::warn_block_literal_attributes_on_omitted_return_type, diag::warn_cconv_unsupported, diag::warn_const_attr_with_pure_attr, diag::warn_cuda_maxclusterrank_sm_90, diag::warn_cxx11_gnu_attribute_on_type, diag::warn_declspec_allocator_nonpointer, diag::warn_declspec_attribute_ignored, diag::warn_deprecated_anonymous_namespace, diag::warn_deprecated_ignored_on_using, diag::warn_dllimport_dropped_from_inline_function, diag::warn_duplicate_attribute, diag::warn_duplicate_attribute_exact, diag::warn_function_attribute_ignored_in_stmt, diag::warn_function_stmt_attribute_precedence, diag::warn_gc_attribute_weak_on_local, diag::warn_gnu_inline_attribute_requires_inline, diag::warn_ignored_ms_inheritance, diag::warn_ignored_objc_externally_retained, diag::warn_import_on_definition, diag::warn_internal_linkage_local_storage, diag::warn_interrupt_attribute_invalid, diag::warn_microsoft_qualifiers_ignored, diag::warn_mig_server_routine_does_not_return_kern_return_t, diag::warn_mismatched_import, diag::warn_mmap_unknown_attribute, diag::warn_nocf_check_attribute_ignored, diag::warn_noderef_on_non_pointer_or_array, diag::warn_nothrow_attribute_ignored, diag::warn_ns_attribute_wrong_parameter_type, diag::warn_ns_attribute_wrong_return_type, diag::warn_objc_direct_ignored, diag::warn_objc_direct_property_ignored, diag::warn_opencl_attr_deprecated_ignored, diag::warn_pure_function_returns_void, diag::warn_require_const_init_added_too_late, diag::warn_riscv_repeated_interrupt_attribute, diag::warn_sycl_kernel_invalid_template_param_type, diag::warn_sycl_kernel_num_of_function_params, diag::warn_sycl_kernel_num_of_template_params, diag::warn_sycl_kernel_return_type, diag::warn_transparent_union_attribute_field_size_align, diag::warn_transparent_union_attribute_floating, diag::warn_transparent_union_attribute_not_definition, diag::warn_transparent_union_attribute_zero_fields, diag::warn_type_attribute_wrong_type, diag::warn_unhandled_ms_attribute_ignored, diag::warn_unsupported_target_attribute, diag::warn_unused_result_typedef_unsupported_spelling, diag::warn_wrong_clang_attr_namespace, -1,
  /* DiagArray407 */ diag::warn_missing_sdksettings_for_availability_checking, -1,
  /* DiagArray408 */ diag::warn_drv_pch_ignoring_gch_dir, diag::warn_drv_pch_ignoring_gch_file, -1,
  /* DiagArray409 */ diag::warn_drv_unsupported_opt_for_target, diag::warn_ignored_gcc_optimization, -1,
  /* DiagArray410 */ diag::warn_pragma_intrinsic_builtin, -1,
  /* DiagArray412 */ diag::warn_clause_expected_string, diag::warn_pragma_align_expected_equal, diag::warn_pragma_align_invalid_option, diag::warn_pragma_comment_ignored, diag::warn_pragma_debug_dependent_argument, diag::warn_pragma_debug_missing_argument, diag::warn_pragma_debug_missing_command, diag::warn_pragma_debug_unexpected_argument, diag::warn_pragma_debug_unexpected_command, diag::warn_pragma_debug_unknown_module, diag::warn_pragma_expected_action_or_r_paren, diag::warn_pragma_expected_colon, diag::warn_pragma_expected_colon_r_paren, diag::warn_pragma_expected_comma, diag::warn_pragma_expected_identifier, diag::warn_pragma_expected_init_seg, diag::warn_pragma_expected_integer, diag::warn_pragma_expected_lparen, diag::warn_pragma_expected_non_wide_string, diag::warn_pragma_expected_predicate, diag::warn_pragma_expected_punc, diag::warn_pragma_expected_rparen, diag::warn_pragma_expected_section_label_or_name, diag::warn_pragma_expected_section_name, diag::warn_pragma_expected_section_push_pop_or_name, diag::warn_pragma_expected_string, diag::warn_pragma_extra_tokens_at_eol, diag::warn_pragma_force_cuda_host_device_bad_arg, diag::warn_pragma_fp_ignored, diag::warn_pragma_init_seg_unsupported_target, diag::warn_pragma_invalid_action, diag::warn_pragma_invalid_argument, diag::warn_pragma_invalid_specific_action, diag::warn_pragma_missing_argument, diag::warn_pragma_ms_fenv_access, diag::warn_pragma_ms_struct, diag::warn_pragma_options_align_reset_failed, diag::warn_pragma_options_expected_align, diag::warn_pragma_pack_invalid_alignment, diag::warn_pragma_pack_malformed, diag::warn_pragma_pop_failed, diag::warn_pragma_pop_macro_no_push, diag::warn_pragma_unknown_extension, diag::warn_pragma_unsupported_action, diag::warn_pragma_unsupported_extension, diag::warn_pragma_unused_expected_var, diag::warn_pragma_unused_expected_var_arg, diag::warn_pragma_unused_undeclared_var, diag::warn_stdc_unknown_rounding_mode, -1,
  /* DiagArray413 */ diag::warn_arc_lifetime_result_type, diag::warn_block_literal_qualifiers_on_omitted_return_type, diag::warn_qual_return_type, diag::warn_typecheck_function_qualifiers_ignored, -1,
  /* DiagArray414 */ diag::warn_typecheck_reference_qualifiers, -1,
  /* DiagArray416 */ diag::warn_auto_implicit_atomic_property, diag::warn_implicit_atomic_property, -1,
  /* DiagArray417 */ diag::warn_impcast_integer_float_precision_constant, -1,
  /* DiagArray418 */ diag::warn_impcast_floating_point_to_bool, -1,
  /* DiagArray419 */ diag::ext_implicit_exception_spec_mismatch, -1,
  /* DiagArray420 */ diag::warn_unannotated_fallthrough, -1,
  /* DiagArray421 */ diag::warn_unannotated_fallthrough_per_function, -1,
  /* DiagArray422 */ diag::warn_impcast_fixed_point_range, -1,
  /* DiagArray423 */ diag::warn_impcast_float_precision, diag::warn_impcast_float_result_precision, -1,
  /* DiagArray424 */ diag::ext_implicit_function_decl_c99, diag::ext_implicit_lib_function_decl, diag::ext_implicit_lib_function_decl_c99, diag::warn_builtin_unknown, diag::warn_implicit_function_decl, -1,
  /* DiagArray425 */ diag::ext_missing_type_specifier, diag::ext_param_not_declared, diag::warn_missing_type_specifier, -1,
  /* DiagArray426 */ diag::warn_impcast_high_order_zero_bits, diag::warn_impcast_integer_precision, -1,
  /* DiagArray427 */ diag::warn_impcast_integer_float_precision, -1,
  /* DiagArray428 */ diag::warn_implicitly_retains_self, -1,
  /* DiagArray429 */ diag::ext_integer_literal_too_large_for_signed, -1,
  /* DiagArray431 */ diag::ext_pp_import_directive, -1,
  /* DiagArray432 */ diag::warn_inaccessible_base_class, -1,
  /* DiagArray433 */ diag::warn_pp_include_angled_in_module_purview, -1,
  /* DiagArray434 */ diag::pp_include_next_absolute_path, -1,
  /* DiagArray435 */ diag::pp_include_next_in_primary, -1,
  /* DiagArray436 */ diag::warn_deep_exception_specs_differ, diag::warn_incompatible_exception_specs, -1,
  /* DiagArray437 */ diag::ext_typecheck_convert_incompatible_function_pointer, -1,
  /* DiagArray438 */ diag::warn_typecheck_convert_incompatible_function_pointer_strict, -1,
  /* DiagArray439 */ diag::warn_redecl_library_builtin, -1,
  /* DiagArray440 */ diag::warn_section_msvc_compat, -1,
  /* DiagArray441 */ diag::warn_cxx_ms_struct, diag::warn_npot_ms_struct, -1,
  /* DiagArray442 */ diag::ext_typecheck_convert_incompatible_pointer, -1,
  /* DiagArray443 */ diag::ext_nested_pointer_qualifier_mismatch, diag::ext_typecheck_convert_discards_qualifiers, diag::warn_bad_cxx_cast_nested_pointer_addr_space, -1,
  /* DiagArray444 */ diag::warn_property_types_are_incompatible, -1,
  /* DiagArray445 */ diag::warn_incompatible_sysroot, -1,
  /* DiagArray446 */ diag::warn_mmap_incomplete_framework_module_declaration, -1,
  /* DiagArray447 */ diag::warn_undef_method_impl, -1,
  /* DiagArray449 */ diag::warn_implicit_decl_no_jmp_buf, -1,
  /* DiagArray450 */ diag::warn_missing_submodule, diag::warn_mmap_umbrella_dir_not_found, diag::warn_uncovered_module_header, -1,
  /* DiagArray451 */ diag::warn_redeclaration_without_attribute_prev_attribute_ignored, diag::warn_redeclaration_without_import_attribute, -1,
  /* DiagArray452 */ diag::warn_inconsistent_destructor_marked_not_override_overriding, -1,
  /* DiagArray453 */ diag::warn_inconsistent_function_marked_not_override_overriding, -1,
  /* DiagArray454 */ diag::ext_increment_bool, -1,
  /* DiagArray455 */ diag::warn_infinite_recursive_function, -1,
  /* DiagArray457 */ diag::ext_initializer_overrides, diag::ext_initializer_union_overrides, diag::warn_initializer_overrides, -1,
  /* DiagArray458 */ diag::ext_out_of_line_qualified_id_type_names_constructor, -1,
  /* DiagArray460 */ diag::warn_fe_inline_asm, -1,
  /* DiagArray461 */ diag::warn_inline_namespace_reopened_noninline, -1,
  /* DiagArray462 */ diag::ext_operator_new_delete_declared_inline, -1,
  /* DiagArray463 */ diag::warn_dylib_symbol_flags_mismatch, diag::warn_glob_did_not_match, diag::warn_header_availability_mismatch, diag::warn_header_hidden_symbol, diag::warn_header_symbol_flags_mismatch, diag::warn_header_symbol_missing, diag::warn_library_hidden_symbol, diag::warn_library_missing_symbol, diag::warn_no_such_excluded_header_file, diag::warn_platform_mismatch, diag::warn_rpaths_mismatch, diag::warn_rpaths_missing, diag::warn_target, -1,
  /* DiagArray464 */ diag::warn_explicit_instantiation_after_specialization, -1,
  /* DiagArray465 */ diag::ext_typecheck_convert_int_pointer, diag::ext_typecheck_convert_pointer_int, -1,
  /* DiagArray467 */ diag::warn_enum_constant_in_bool_context, diag::warn_left_shift_in_bool_context, -1,
  /* DiagArray468 */ diag::warn_int_to_pointer_cast, -1,
  /* DiagArray469 */ diag::warn_int_to_void_pointer_cast, -1,
  /* DiagArray470 */ diag::warn_integer_constant_overflow, -1,
  /* DiagArray471 */ diag::warn_drv_missing_plugin_arg, diag::warn_drv_missing_plugin_name, diag::warn_drv_msp430_hwmult_mismatch, diag::warn_drv_msp430_hwmult_no_device, diag::warn_drv_msp430_hwmult_unsupported, diag::warn_drv_object_size_disabled_O0, diag::warn_drv_optimization_value, diag::warn_fe_backend_invalid_feature_flag, diag::warn_fe_backend_readonly_feature_flag, diag::warn_target_unrecognized_env, diag::warn_target_unsupported_extension, -1,
  /* DiagArray472 */ diag::ext_constexpr_function_never_constant_expr, -1,
  /* DiagArray473 */ diag::warn_invalid_feature_combination, -1,
  /* DiagArray474 */ diag::warn_iboutlet_object_type, diag::warn_iboutletcollection_property_assign, -1,
  /* DiagArray475 */ diag::warn_invalid_initializer_from_system_header, -1,
  /* DiagArray476 */ diag::warn_invalid_ios_deployment_target, -1,
  /* DiagArray477 */ diag::warn_attribute_no_builtin_invalid_builtin_name, -1,
  /* DiagArray478 */ diag::warn_falloff_noreturn_function, diag::warn_noreturn_function_has_return_expr, -1,
  /* DiagArray479 */ diag::ext_offsetof_non_pod_type, diag::ext_offsetof_non_standardlayout_type, -1,
  /* DiagArray480 */ diag::warn_drv_unable_to_find_directory_expected, -1,
  /* DiagArray481 */ diag::ext_partial_spec_not_more_specialized_than_primary, -1,
  /* DiagArray483 */ diag::ext_empty_character, diag::ext_unterminated_char_or_string, -1,
  /* DiagArray484 */ diag::warn_bad_character_encoding, diag::warn_bad_string_encoding, -1,
  /* DiagArray485 */ diag::warn_static_assert_message_constexpr, -1,
  /* DiagArray486 */ diag::ext_pp_bad_paste_ms, -1,
  /* DiagArray487 */ diag::warn_unevaluated_string_prefix, -1,
  /* DiagArray488 */ diag::warn_invalid_utf8_in_comment, -1,
  /* DiagArray489 */ diag::warn_jump_out_of_seh_finally, -1,
  /* DiagArray490 */ diag::ext_keyword_as_ident, -1,
  /* DiagArray491 */ diag::warn_pp_macro_hides_keyword, -1,
  /* DiagArray492 */ diag::ext_param_promoted_not_compatible_with_prototype, -1,
  /* DiagArray493 */ diag::ext_token_used, -1,
  /* DiagArray494 */ diag::warn_parameter_size, diag::warn_return_value_size, -1,
  /* DiagArray496 */ diag::warn_fe_linking_module, -1,
  /* DiagArray497 */ diag::warn_impcast_literal_float_to_integer, diag::warn_impcast_literal_float_to_integer_out_of_range, -1,
  /* DiagArray498 */ diag::warn_float_compare_literal, diag::warn_float_overflow, diag::warn_float_underflow, -1,
  /* DiagArray499 */ diag::ext_template_arg_local_type, -1,
  /* DiagArray500 */ diag::warn_logical_not_on_lhs_of_check, -1,
  /* DiagArray501 */ diag::warn_logical_and_in_logical_or, -1,
  /* DiagArray502 */ diag::ext_c99_longlong, -1,
  /* DiagArray504 */ diag::ext_pp_macro_redef, -1,
  /* DiagArray505 */ diag::ext_main_used, diag::ext_noreturn_main, diag::ext_variadic_main, diag::warn_main_one_arg, diag::warn_main_redefined, diag::warn_main_returns_bool_literal, diag::warn_static_main, -1,
  /* DiagArray506 */ diag::ext_main_returns_nonint, -1,
  /* DiagArray507 */ diag::warn_has_warning_invalid_option, -1,
  /* DiagArray508 */ diag::ext_many_braces_around_init, -1,
  /* DiagArray509 */ diag::ext_mathematical_notation, -1,
  /* DiagArray510 */ diag::warn_max_tokens, diag::warn_max_tokens_total, -1,
  /* DiagArray511 */ diag::warn_max_unsigned_zero, -1,
  /* DiagArray512 */ diag::warn_suspicious_sizeof_memset, -1,
  /* DiagArray513 */ diag::warn_memsize_comparison, -1,
  /* DiagArray514 */ diag::warn_non_contravariant_param_types, diag::warn_non_covariant_ret_types, -1,
  /* DiagArray516 */ diag::ext_ms_abstract_keyword, -1,
  /* DiagArray517 */ diag::ext_anonymous_record_with_type, diag::ext_ms_anonymous_record, -1,
  /* DiagArray518 */ diag::ext_ms_cast_fn_obj, diag::ext_ms_impcast_fn_obj, -1,
  /* DiagArray519 */ diag::ext_charize_microsoft, -1,
  /* DiagArray520 */ diag::ext_comment_paste_microsoft, -1,
  /* DiagArray521 */ diag::ext_default_init_const, -1,
  /* DiagArray522 */ diag::ext_pp_operator_used_as_macro_name, -1,
  /* DiagArray523 */ diag::ext_param_default_argument_redefinition, -1,
  /* DiagArray524 */ diag::warn_attribute_section_drectve, -1,
  /* DiagArray525 */ diag::ext_ctrl_z_eof_microsoft, -1,
  /* DiagArray526 */ diag::ext_ms_forward_ref_enum, -1,
  /* DiagArray527 */ diag::ext_enumerator_too_large, -1,
  /* DiagArray528 */ diag::ext_ellipsis_exception_spec, diag::ext_incomplete_in_exception_spec, diag::ext_mismatched_exception_spec, diag::ext_mismatched_exception_spec_explicit_instantiation, diag::ext_override_exception_spec, -1,
  /* DiagArray529 */ diag::warn_microsoft_dependent_exists, -1,
  /* DiagArray530 */ diag::ext_ms_explicit_constructor_call, -1,
  /* DiagArray531 */ diag::warn_member_extra_qualification, -1,
  /* DiagArray532 */ diag::ext_ms_c_enum_fixed_underlying_type, -1,
  /* DiagArray533 */ diag::ext_flexible_array_empty_aggregate_ms, diag::ext_flexible_array_union_ms, -1,
  /* DiagArray534 */ diag::ext_goto_into_protected_scope, -1,
  /* DiagArray535 */ diag::ext_ms_ambiguous_direct_base, -1,
  /* DiagArray536 */ diag::ext_pp_include_search_ms, -1,
  /* DiagArray537 */ diag::ext_init_from_predefined, -1,
  /* DiagArray538 */ diag::ext_mutable_reference, -1,
  /* DiagArray539 */ diag::ext_pure_function_definition, -1,
  /* DiagArray540 */ diag::ext_static_non_static, -1,
  /* DiagArray541 */ diag::ext_ms_sealed_keyword, -1,
  /* DiagArray542 */ diag::ext_ms_static_assert, -1,
  /* DiagArray543 */ diag::ext_string_literal_from_predefined, -1,
  /* DiagArray544 */ diag::ext_explicit_instantiation_duplicate, diag::ext_found_in_dependent_base, diag::ext_found_later_in_class, diag::ext_ms_delayed_template_argument, diag::ext_ms_deref_template_argument, diag::ext_ms_template_spec_redecl_out_of_scope, diag::ext_ms_template_type_arg_missing_typename, diag::ext_static_out_of_line, diag::ext_undeclared_unqual_id_with_dependent_base, diag::ext_unqualified_base_class, -1,
  /* DiagArray545 */ diag::ext_template_param_shadow, -1,
  /* DiagArray546 */ diag::ext_union_member_of_reference_type, -1,
  /* DiagArray547 */ diag::ext_friend_tag_redecl_outside_namespace, -1,
  /* DiagArray548 */ diag::ext_ms_using_declaration_inaccessible, -1,
  /* DiagArray549 */ diag::ext_pseudo_dtor_on_void, -1,
  /* DiagArray550 */ diag::warn_profile_data_misexpect, -1,
  /* DiagArray551 */ diag::warn_misleading_indentation, -1,
  /* DiagArray552 */ diag::warn_mismatched_delete_new, -1,
  /* DiagArray553 */ diag::warn_conflicting_param_types, -1,
  /* DiagArray554 */ diag::warn_conflicting_ret_types, -1,
  /* DiagArray555 */ diag::warn_struct_class_previous_tag_mismatch, diag::warn_struct_class_tag_mismatch, -1,
  /* DiagArray556 */ diag::warn_missing_braces, -1,
  /* DiagArray557 */ diag::ext_constinit_missing, -1,
  /* DiagArray558 */ diag::ext_no_declarators, diag::ext_standalone_specifier, diag::ext_typedef_without_a_name, diag::warn_standalone_specifier, -1,
  /* DiagArray559 */ diag::warn_missing_designated_field_initializers, -1,
  /* DiagArray560 */ diag::ext_missing_exception_specification, -1,
  /* DiagArray561 */ diag::warn_missing_field_initializers, -1,
  /* DiagArray564 */ diag::warn_missing_method_return_type, -1,
  /* DiagArray565 */ diag::warn_drv_missing_multilib, -1,
  /* DiagArray566 */ diag::warn_overriding_method_missing_noescape, -1,
  /* DiagArray567 */ diag::warn_suggest_noreturn_block, diag::warn_suggest_noreturn_function, -1,
  /* DiagArray568 */ diag::warn_cconv_knr, -1,
  /* DiagArray569 */ diag::warn_missing_prototype, -1,
  /* DiagArray570 */ diag::warn_missing_selector_name, -1,
  /* DiagArray571 */ diag::warn_missing_sysroot, -1,
  /* DiagArray572 */ diag::warn_missing_variable_declarations, -1,
  /* DiagArray573 */ diag::warn_omp_assume_attribute_string_unknown_suggested, -1,
  /* DiagArray574 */ diag::warn_hlsl_packoffset_mix, -1,
  /* DiagArray575 */ diag::remark_module_build, diag::remark_module_build_done, diag::remark_module_lock_failure, diag::remark_module_lock_timeout, -1,
  /* DiagArray576 */ diag::warn_module_conflict, diag::warn_module_system_bit_conflict, -1,
  /* DiagArray577 */ diag::warn_module_config_mismatch, -1,
  /* DiagArray578 */ diag::warn_duplicate_module_file_extension, -1,
  /* DiagArray579 */ diag::remark_module_import, -1,
  /* DiagArray580 */ diag::ext_module_import_in_extern_c, -1,
  /* DiagArray581 */ diag::remark_pp_include_directive_modular_translation, -1,
  /* DiagArray582 */ diag::remark_module_lock, -1,
  /* DiagArray583 */ diag::ext_equivalent_internal_linkage_decl_in_modules, -1,
  /* DiagArray584 */ diag::ext_module_import_not_at_top_level_noop, -1,
  /* DiagArray588 */ diag::warn_drv_msvc_not_found, -1,
  /* DiagArray589 */ diag::warn_drv_multi_gpu_arch, -1,
  /* DiagArray590 */ diag::warn_multichar_character_literal, -1,
  /* DiagArray591 */ diag::warn_vbase_moved_multiple_times, -1,
  /* DiagArray592 */ diag::warn_fp_nan_inf_when_disabled, -1,
  /* DiagArray594 */ diag::ext_anonymous_record_with_anonymous_type, -1,
  /* DiagArray596 */ diag::warn_operator_new_returns_null, -1,
  /* DiagArray597 */ diag::ext_no_newline_eof, diag::warn_no_newline_eof, -1,
  /* DiagArray598 */ diag::warn_dereference_of_noderef_type, diag::warn_dereference_of_noderef_type_no_decl, diag::warn_noderef_to_dereferenceable_pointer, -1,
  /* DiagArray600 */ diag::ext_non_c_like_anon_struct_in_typedef, -1,
  /* DiagArray602 */ diag::warn_non_literal_null_pointer, -1,
  /* DiagArray603 */ diag::warn_non_modular_include_in_framework_module, -1,
  /* DiagArray604 */ diag::warn_non_modular_include_in_module, -1,
  /* DiagArray605 */ diag::warn_cannot_pass_non_pod_arg_to_vararg, diag::warn_non_pod_vararg_with_format_string, diag::warn_second_parameter_to_va_arg_not_pod, diag::warn_second_parameter_to_va_arg_ownership_qualified, -1,
  /* DiagArray606 */ diag::warn_alignment_not_power_of_two, -1,
  /* DiagArray607 */ diag::warn_non_virtual_dtor, -1,
  /* DiagArray608 */ diag::warn_null_arg, diag::warn_null_ret, -1,
  /* DiagArray610 */ diag::pp_nonportable_path, -1,
  /* DiagArray611 */ diag::warn_apinotes_private_case, -1,
  /* DiagArray612 */ diag::warn_apinotes_private_case_system, -1,
  /* DiagArray613 */ diag::pp_nonportable_system_path, -1,
  /* DiagArray614 */ diag::warn_neon_vector_initializer_non_portable, -1,
  /* DiagArray615 */ diag::warn_cstruct_memaccess, -1,
  /* DiagArray616 */ diag::warn_nsconsumed_attribute_mismatch, -1,
  /* DiagArray617 */ diag::warn_nsreturns_retained_attribute_mismatch, -1,
  /* DiagArray618 */ diag::warn_null_in_arithmetic_operation, diag::warn_null_in_comparison_operation, -1,
  /* DiagArray619 */ diag::null_in_char_or_string, diag::null_in_file, -1,
  /* DiagArray620 */ diag::warn_impcast_null_pointer_to_integer, -1,
  /* DiagArray621 */ diag::warn_binding_null_to_reference, diag::warn_indirection_through_null, -1,
  /* DiagArray622 */ diag::warn_pointer_arith_null_ptr, -1,
  /* DiagArray623 */ diag::warn_pointer_sub_null_ptr, -1,
  /* DiagArray624 */ diag::warn_conflicting_nullability_attr_overriding_param_types, diag::warn_conflicting_nullability_attr_overriding_ret_types, diag::warn_mismatched_nullability_attr, diag::warn_null_resettable_setter, diag::warn_nullability_duplicate, -1,
  /* DiagArray625 */ diag::warn_nullability_missing, -1,
  /* DiagArray626 */ diag::warn_nullability_missing_array, -1,
  /* DiagArray627 */ diag::warn_nullability_declspec, -1,
  /* DiagArray628 */ diag::ext_nullability, -1,
  /* DiagArray629 */ diag::warn_nullability_inferred_on_nested_type, -1,
  /* DiagArray630 */ diag::warn_nullability_lost, -1,
  /* DiagArray631 */ diag::warn_autosynthesis_property_ivar_match, -1,
  /* DiagArray632 */ diag::warn_impcast_constant_value_to_objc_bool, -1,
  /* DiagArray633 */ diag::warn_objc_boxing_invalid_utf8_string, -1,
  /* DiagArray634 */ diag::warn_objc_circular_container, -1,
  /* DiagArray636 */ diag::warn_objc_designated_init_missing_super_call, diag::warn_objc_designated_init_non_designated_init_call, diag::warn_objc_designated_init_non_super_designated_init_call, diag::warn_objc_implementation_missing_designated_init_override, diag::warn_objc_secondary_init_missing_init_call, diag::warn_objc_secondary_init_super_init_call, -1,
  /* DiagArray637 */ diag::warn_nsdictionary_duplicate_key, -1,
  /* DiagArray638 */ diag::warn_dup_category_def, -1,
  /* DiagArray639 */ diag::warn_superclass_variable_sized_type_not_at_end, diag::warn_variable_sized_ivar_visibility, -1,
  /* DiagArray640 */ diag::warn_forward_class_redefinition, -1,
  /* DiagArray641 */ diag::warn_ivars_in_interface, -1,
  /* DiagArray642 */ diag::warn_objc_literal_comparison, -1,
  /* DiagArray643 */ diag::warn_impcast_objective_c_literal_to_bool, diag::warn_objc_collection_literal_element, -1,
  /* DiagArray644 */ diag::warn_pp_objc_macro_redef_ignored, -1,
  /* DiagArray645 */ diag::warn_messaging_unqualified_id, -1,
  /* DiagArray646 */ diag::warn_class_method_not_found, diag::warn_class_method_not_found_with_typo, diag::warn_inst_method_not_found, diag::warn_instance_method_not_found_with_typo, diag::warn_instance_method_on_class_found, diag::warn_root_inst_method_not_found, -1,
  /* DiagArray647 */ diag::warn_missing_explicit_synthesis, -1,
  /* DiagArray648 */ diag::warn_objc_missing_super_call, -1,
  /* DiagArray649 */ diag::warn_multiple_method_decl, -1,
  /* DiagArray650 */ diag::warn_objc_property_retain_of_block, -1,
  /* DiagArray651 */ diag::warn_objc_pointer_cxx_catch_fragile, -1,
  /* DiagArray652 */ diag::warn_objc_property_assign_on_object, -1,
  /* DiagArray653 */ diag::warn_impl_required_for_class_property, diag::warn_impl_required_in_category_for_class_property, diag::warn_setter_getter_impl_required, diag::warn_setter_getter_impl_required_in_category, -1,
  /* DiagArray654 */ diag::warn_property_implicitly_mismatched, -1,
  /* DiagArray655 */ diag::warn_cocoa_naming_owned_rule, -1,
  /* DiagArray656 */ diag::warn_objc_property_default_assign_on_object, diag::warn_objc_property_no_assignment_attribute, -1,
  /* DiagArray657 */ diag::warn_autosynthesis_property_in_superclass, diag::warn_no_autosynthesis_property, diag::warn_no_autosynthesis_shared_ivar_property, -1,
  /* DiagArray658 */ diag::warn_category_method_impl_match, -1,
  /* DiagArray659 */ diag::warn_auto_synthesizing_protocol_property, -1,
  /* DiagArray660 */ diag::warn_objc_redundant_qualified_class_type, -1,
  /* DiagArray661 */ diag::warn_objc_readonly_property_has_setter, -1,
  /* DiagArray663 */ diag::warn_objc_redundant_literal_use, -1,
  /* DiagArray664 */ diag::warn_objc_root_class_missing, -1,
  /* DiagArray666 */ diag::warn_impcast_float_to_objc_signed_char_bool, -1,
  /* DiagArray667 */ diag::warn_impcast_int_to_objc_signed_char_bool, -1,
  /* DiagArray668 */ diag::warn_objc_string_literal_comparison, -1,
  /* DiagArray669 */ diag::warn_concatenated_nsarray_literal, -1,
  /* DiagArray670 */ diag::warn_objc_unsafe_perform_selector, -1,
  /* DiagArray671 */ diag::warn_odr_different_num_template_parameters, diag::warn_odr_different_template_parameter_kind, diag::warn_odr_field_type_inconsistent, diag::warn_odr_function_type_inconsistent, diag::warn_odr_ivar_type_inconsistent, diag::warn_odr_non_type_parameter_type_inconsistent, diag::warn_odr_objc_method_num_params_inconsistent, diag::warn_odr_objc_method_param_type_inconsistent, diag::warn_odr_objc_method_result_type_inconsistent, diag::warn_odr_objc_method_variadic_inconsistent, diag::warn_odr_objc_property_impl_kind_inconsistent, diag::warn_odr_objc_property_type_inconsistent, diag::warn_odr_objc_superclass_inconsistent, diag::warn_odr_objc_synthesize_ivar_inconsistent, diag::warn_odr_parameter_pack_non_pack, diag::warn_odr_tag_type_inconsistent, diag::warn_odr_variable_multiple_def, diag::warn_odr_variable_type_inconsistent, -1,
  /* DiagArray672 */ diag::warn_old_style_cast, -1,
  /* DiagArray675 */ diag::warn_acc_deprecated_alias_name, -1,
  /* DiagArray676 */ diag::warn_acc_if_self_conflict, -1,
  /* DiagArray677 */ diag::ext_opencl_ext_vector_type_rgba_selector, -1,
  /* DiagArray679 */ diag::ext_omp_attributes, -1,
  /* DiagArray680 */ diag::warn_omp_alignment_not_power_of_two, diag::warn_omp_allocate_thread_on_task_target_directive, diag::warn_omp_ctx_incompatible_property_for_selector, diag::warn_omp_ctx_incompatible_score_for_property, diag::warn_omp_ctx_incompatible_selector_for_set, diag::warn_omp_ctx_selector_without_properties, diag::warn_omp_declare_variant_ctx_mutiple_use, diag::warn_omp_declare_variant_ctx_not_a_property, diag::warn_omp_declare_variant_ctx_not_a_selector, diag::warn_omp_declare_variant_ctx_not_a_set, diag::warn_omp_declare_variant_expected, diag::warn_omp_declare_variant_string_literal_or_identifier, diag::warn_omp_linear_step_zero, diag::warn_omp_more_one_device_type_clause, diag::warn_omp_more_one_interop_type, diag::warn_omp_more_one_omp_all_memory, diag::warn_omp_unknown_assumption_clause_missing_id, diag::warn_omp_unknown_assumption_clause_without_args, diag::warn_omp_used_different_allocator, -1,
  /* DiagArray681 */ diag::warn_omp_invalid_attribute_for_ompx_attributes, -1,
  /* DiagArray682 */ diag::ext_omp_loop_not_canonical_init, diag::warn_omp_loop_64_bit_var, -1,
  /* DiagArray683 */ diag::warn_omp_non_trivial_type_mapped, -1,
  /* DiagArray684 */ diag::warn_drv_omp_offload_target_duplicate, diag::warn_omp_declare_target_after_first_use, diag::warn_omp_not_in_target_context, -1,
  /* DiagArray685 */ diag::warn_throw_not_valid_on_target, diag::warn_try_not_valid_on_target, -1,
  /* DiagArray686 */ diag::warn_drv_fine_grained_bitfield_accesses_ignored, diag::warn_drv_fjmc_for_elf_only, diag::warn_drv_jmc_requires_debuginfo, diag::warn_drv_loongarch_conflicting_implied_val, diag::warn_drv_moutline_atomics_unsupported_opt, diag::warn_drv_moutline_unsupported_opt, diag::warn_drv_needs_hvx, diag::warn_drv_ps_force_pic, diag::warn_drv_unsupported_diag_option_for_flang, diag::warn_drv_unsupported_longcalls, diag::warn_drv_unsupported_openmp_library, diag::warn_drv_unsupported_option_for_flang, diag::warn_drv_unsupported_option_for_offload_arch_req_feature, diag::warn_drv_unsupported_option_for_processor, diag::warn_drv_unsupported_option_for_target, diag::warn_drv_unsupported_pic_with_mabicalls, diag::warn_drv_unsupported_sdata, diag::warn_target_override_arm64ec, -1,
  /* DiagArray687 */ diag::ext_typecheck_ordered_comparison_of_function_pointers, diag::warn_typecheck_ordered_comparison_of_function_pointers, -1,
  /* DiagArray688 */ diag::ext_out_of_line_declaration, -1,
  /* DiagArray689 */ diag::ext_use_out_of_scope_declaration, -1,
  /* DiagArray690 */ diag::warn_overaligned_type, -1,
  /* DiagArray692 */ diag::ext_string_too_long, -1,
  /* DiagArray693 */ diag::warn_overloaded_shift_in_comparison, -1,
  /* DiagArray694 */ diag::warn_overloaded_virtual, -1,
  /* DiagArray696 */ diag::warn_fe_override_module, -1,
  /* DiagArray697 */ diag::warn_conflicting_overriding_param_modifiers, diag::warn_conflicting_overriding_param_types, diag::warn_conflicting_overriding_ret_type_modifiers, diag::warn_conflicting_overriding_ret_types, diag::warn_conflicting_overriding_variadic, diag::warn_non_contravariant_overriding_param_types, diag::warn_non_covariant_overriding_ret_types, -1,
  /* DiagArray698 */ diag::warn_drv_overriding_option, -1,
  /* DiagArray699 */ diag::warn_unnecessary_packed, -1,
  /* DiagArray700 */ diag::warn_unpacked_field, -1,
  /* DiagArray701 */ diag::warn_padded_struct_anon_field, diag::warn_padded_struct_field, diag::warn_padded_struct_size, -1,
  /* DiagArray702 */ diag::warn_padded_struct_anon_bitfield, diag::warn_padded_struct_bitfield, -1,
  /* DiagArray703 */ diag::warn_condition_is_assignment, diag::warn_consecutive_comparison, diag::warn_precedence_bitwise_rel, diag::warn_precedence_conditional, -1,
  /* DiagArray704 */ diag::warn_equality_with_extra_parens, -1,
  /* DiagArray706 */ diag::remark_fe_backend_optimization_remark, -1,
  /* DiagArray707 */ diag::remark_fe_backend_optimization_remark_analysis, diag::remark_fe_backend_optimization_remark_analysis_aliasing, diag::remark_fe_backend_optimization_remark_analysis_fpcommute, -1,
  /* DiagArray708 */ diag::warn_fe_backend_optimization_failure, -1,
  /* DiagArray709 */ diag::remark_fe_backend_optimization_remark_missed, -1,
  /* DiagArray710 */ diag::warn_module_uses_date_time, -1,
  /* DiagArray711 */ diag::warn_pch_vfsoverlay_mismatch, -1,
  /* DiagArray712 */ diag::ext_aggregate_init_not_constant, diag::ext_anonymous_record_with_type, diag::ext_anonymous_struct_union_qualified, diag::ext_array_size_conversion, diag::ext_auto_new_list_init, diag::ext_binary_literal, diag::ext_c99_array_usage, diag::ext_c99_compound_literal, diag::ext_c99_feature, diag::ext_c99_flexible_array_member, diag::ext_c99_variable_decl_in_for_loop, diag::ext_c_empty_initializer, diag::ext_c_missing_varargs_arg, diag::ext_c_nullptr, diag::ext_cast_fn_obj, diag::ext_clang_diagnose_if, diag::ext_clang_enable_if, diag::ext_cxx11_enum_fixed_underlying_type, diag::ext_cxx_missing_varargs_arg, diag::ext_designated_init, diag::ext_dtor_name_ambiguous, diag::ext_dtor_named_in_wrong_scope, diag::ext_duplicate_declspec, diag::ext_ellipsis_exception_spec, diag::ext_empty_fnmacro_arg, diag::ext_enum_value_not_int, diag::ext_enumerator_list_comma_c, diag::ext_enumerator_list_comma_cxx, diag::ext_explicit_instantiation_without_qualified_id, diag::ext_expr_not_ice, diag::ext_extern_template, diag::ext_extra_semi, diag::ext_forward_ref_enum, diag::ext_freestanding_complex, diag::ext_gnu_array_range, diag::ext_hex_constant_invalid, diag::ext_hex_literal_invalid, diag::ext_ident_list_in_param, diag::ext_in_class_initializer_non_constant, diag::ext_increment_complex, diag::ext_integer_complement_complex, diag::ext_internal_in_extern_inline_quiet, diag::ext_line_comment, diag::ext_main_used, diag::ext_mixed_decls_code, diag::ext_multi_line_line_comment, diag::ext_named_variadic_macro, diag::ext_no_newline_eof, diag::ext_nonstandard_escape, diag::ext_ns_enum_attribute, diag::ext_opencl_double_without_pragma, diag::ext_pointer_to_const_ref_member_on_rvalue, diag::ext_pp_bad_vaargs_use, diag::ext_pp_comma_expr, diag::ext_pp_ident_directive, diag::ext_pp_line_too_big, diag::ext_pp_opencl_variadic_macros, diag::ext_pp_warning_directive, diag::ext_return_has_void_expr, diag::ext_rvalue_to_reference_access_ctor, diag::ext_rvalue_to_reference_temp_copy_no_viable, diag::ext_sizeof_alignof_function_type, diag::ext_sizeof_alignof_void_type, diag::ext_subscript_non_lvalue, diag::ext_thread_before, diag::ext_type_defined_in_offsetof, diag::ext_typecheck_addrof_void, diag::ext_typecheck_cast_nonscalar, diag::ext_typecheck_comparison_of_fptr_to_void, diag::ext_typecheck_cond_one_void, diag::ext_typecheck_convert_pointer_void_func, diag::ext_typecheck_ordered_comparison_of_pointer_and_zero, diag::ext_variadic_macro, diag::ext_vla, diag::ext_vla_cxx_in_gnu_mode, diag::ext_vla_cxx_in_gnu_mode_static_assert, diag::warn_defined_in_function_type_macro, diag::warn_ext_c23_attributes, diag::warn_ext_cxx11_attributes, diag::warn_format_conversion_argument_type_mismatch_pedantic, diag::warn_kern_is_method, diag::warn_strict_prototypes, -1,
  /* DiagArray713 */ diag::warn_opencl_unsupported_core_feature, diag::warn_pragma_extension_is_core, -1,
  /* DiagArray715 */ diag::warn_pessimizing_move_on_initialization, diag::warn_pessimizing_move_on_return, -1,
  /* DiagArray716 */ diag::ext_sizeof_alignof_function_type, diag::ext_sizeof_alignof_void_type, diag::warn_sub_ptr_zero_size_types, -1,
  /* DiagArray717 */ diag::warn_cast_nonnull_to_bool, diag::warn_impcast_pointer_to_bool, -1,
  /* DiagArray718 */ diag::warn_pointer_compare, -1,
  /* DiagArray719 */ diag::ext_typecheck_comparison_of_pointer_integer, -1,
  /* DiagArray720 */ diag::ext_typecheck_convert_incompatible_pointer_sign, -1,
  /* DiagArray721 */ diag::warn_pointer_to_enum_cast, -1,
  /* DiagArray722 */ diag::warn_pointer_to_int_cast, -1,
  /* DiagArray723 */ diag::ext_typecheck_cond_incompatible_pointers, -1,
  /* DiagArray724 */ diag::warn_poison_system_directories, -1,
  /* DiagArray725 */ diag::warn_potentially_direct_selector_expression, -1,
  /* DiagArray726 */ diag::warn_side_effects_typeid, -1,
  /* DiagArray727 */ diag::warn_pragma_attribute_unused, -1,
  /* DiagArray728 */ diag::pp_pragma_once_in_main_file, -1,
  /* DiagArray729 */ diag::warn_pragma_pack_modified_after_include, diag::warn_pragma_pack_no_pop_eof, -1,
  /* DiagArray730 */ diag::warn_pragma_pack_non_default_at_include, -1,
  /* DiagArray731 */ diag::pp_pragma_sysheader_in_main_file, -1,
  /* DiagArray732 */ diag::warn_no_support_for_eval_method_source_on_m32, diag::warn_redefine_extname_not_applied, -1,
  /* DiagArray733 */ diag::warn_cxx11_compat_constexpr_body_invalid_stmt, diag::warn_cxx11_compat_constexpr_body_multiple_return, diag::warn_cxx11_compat_constexpr_body_no_return, diag::warn_cxx11_compat_constexpr_local_var, diag::warn_cxx11_compat_constexpr_type_definition, diag::warn_cxx11_compat_decltype_auto_type_specifier, diag::warn_cxx11_compat_deduced_return_type, diag::warn_cxx11_compat_digit_separator, diag::warn_cxx11_compat_generic_lambda, diag::warn_cxx11_compat_init_capture, diag::warn_cxx11_compat_variable_template, -1,
  /* DiagArray735 */ diag::warn_cxx14_compat_class_template_argument_deduction, diag::warn_cxx14_compat_constexpr_if, diag::warn_cxx14_compat_constexpr_on_lambda, diag::warn_cxx14_compat_decomp_decl, diag::warn_cxx14_compat_fold_expression, diag::warn_cxx14_compat_init_statement, diag::warn_cxx14_compat_inline_variable, diag::warn_cxx14_compat_nested_namespace_definition, diag::warn_cxx14_compat_star_this_lambda_capture, diag::warn_cxx14_compat_static_assert_no_message, diag::warn_cxx14_compat_template_nontype_parm_auto_type, diag::warn_cxx14_compat_template_template_param_typename, diag::warn_cxx14_compat_u8_character_literal, diag::warn_cxx14_compat_using_attribute_ns, diag::warn_cxx17_compat_multi_using_declaration, diag::warn_cxx17_compat_using_declaration_pack, diag::warn_for_range_begin_end_types_differ, -1,
  /* DiagArray736 */ diag::warn_cxx14_compat_ns_enum_attribute, diag::warn_cxx17_hex_literal, -1,
  /* DiagArray737 */ diag::warn_cxx17_compat_adl_only_template_id, diag::warn_cxx17_compat_bitfield_member_init, diag::warn_cxx17_compat_capture_binding, diag::warn_cxx17_compat_constexpr_body_invalid_stmt, diag::warn_cxx17_compat_constexpr_ctor_missing_init, diag::warn_cxx17_compat_constexpr_function_try_block, diag::warn_cxx17_compat_constexpr_local_var_no_init, diag::warn_cxx17_compat_constexpr_union_ctor_no_init, diag::warn_cxx17_compat_constexpr_virtual, diag::warn_cxx17_compat_ctad_for_alias_templates, diag::warn_cxx17_compat_decomp_decl_spec, diag::warn_cxx17_compat_defaulted_comparison, diag::warn_cxx17_compat_defaulted_method_type_mismatch, diag::warn_cxx17_compat_equals_this_lambda_capture, diag::warn_cxx17_compat_explicit_bool, diag::warn_cxx17_compat_for_range_init_stmt, diag::warn_cxx17_compat_init_capture_pack, diag::warn_cxx17_compat_inline_nested_namespace_definition, diag::warn_cxx17_compat_lambda_def_ctor_assign, diag::warn_cxx17_compat_lambda_template_parameter_list, diag::warn_cxx17_compat_missing_varargs_arg, diag::warn_cxx17_compat_spaceship, diag::warn_cxx17_compat_template_nontype_parm_type, diag::warn_cxx17_compat_unicode_type, diag::warn_cxx17_compat_using_decl_class_member_enumerator, diag::warn_cxx17_compat_using_decl_non_member_enumerator, diag::warn_cxx17_compat_using_decl_scoped_enumerator, diag::warn_cxx17_compat_using_enum_declaration, -1,
  /* DiagArray738 */ diag::warn_cxx17_compat_designated_init, diag::warn_cxx17_compat_pointer_to_const_ref_member_on_rvalue, -1,
  /* DiagArray739 */ diag::ext_subscript_overload, diag::warn_cxx20_alias_in_init_statement, diag::warn_cxx20_compat_auto_expr, diag::warn_cxx20_compat_consteval_if, diag::warn_cxx20_compat_constexpr_body_invalid_stmt, diag::warn_cxx20_compat_constexpr_var, diag::warn_cxx20_compat_decl_attrs_on_lambda, diag::warn_cxx20_compat_label_end_of_compound_statement, diag::warn_cxx20_compat_operator_overload_static, diag::warn_cxx20_compat_size_t_suffix, diag::warn_cxx20_compat_static_lambda, diag::warn_cxx23_compat_pp_directive, diag::warn_cxx23_compat_warning_directive, diag::warn_cxx23_delimited_escape_sequence, -1,
  /* DiagArray741 */ diag::warn_cxx23_compat_decl_attrs_on_binding, diag::warn_cxx23_delete_with_message, diag::warn_cxx23_pack_indexing, diag::warn_cxx23_placeholder_var_definition, -1,
  /* DiagArray745 */ diag::warn_c11_compat_keyword, -1,
  /* DiagArray747 */ diag::warn_c17_compat_ellipsis_only_parameter, diag::warn_c17_compat_missing_varargs_arg, diag::warn_c17_compat_static_assert_no_message, diag::warn_c23_compat_binary_literal, diag::warn_c23_compat_bitint_suffix, diag::warn_c23_compat_digit_separator, diag::warn_c23_compat_empty_initializer, diag::warn_c23_compat_keyword, diag::warn_c23_compat_label_end_of_compound_statement, diag::warn_c23_compat_label_followed_by_declaration, diag::warn_c23_compat_literal_ucn_control_character, diag::warn_c23_compat_literal_ucn_escape_basic_scs, diag::warn_c23_compat_pp_directive, diag::warn_c23_compat_warning_directive, diag::warn_pre_c23_compat_attributes, -1,
  /* DiagArray751 */ diag::warn_omp51_compat_attributes, -1,
  /* DiagArray752 */ diag::ext_predef_outside_function, -1,
  /* DiagArray753 */ diag::warn_private_extern, -1,
  /* DiagArray754 */ diag::warn_use_of_private_header_outside_module, -1,
  /* DiagArray755 */ diag::warn_mmap_mismatched_private_module_name, diag::warn_mmap_mismatched_private_submodule, diag::warn_mmap_redundant_export_as, diag::warn_no_priv_submodule_use_toplevel, -1,
  /* DiagArray756 */ diag::warn_profile_data_missing, -1,
  /* DiagArray757 */ diag::warn_profile_data_out_of_date, -1,
  /* DiagArray758 */ diag::warn_profile_data_unprofiled, -1,
  /* DiagArray759 */ diag::warn_property_access_suggest, -1,
  /* DiagArray760 */ diag::warn_property_attr_mismatch, diag::warn_property_attribute, diag::warn_property_redecl_getter_mismatch, diag::warn_readonly_property, -1,
  /* DiagArray761 */ diag::warn_unimplemented_protocol_method, -1,
  /* DiagArray762 */ diag::warn_protocol_property_mismatch, -1,
  /* DiagArray763 */ diag::warn_avx_calling_convention, -1,
  /* DiagArray764 */ diag::warn_ptrauth_auth_null_pointer, diag::warn_ptrauth_sign_null_pointer, -1,
  /* DiagArray765 */ diag::err_func_returning_qualified_void, -1,
  /* DiagArray766 */ diag::warn_quoted_include_in_framework_header, -1,
  /* DiagArray768 */ diag::warn_for_range_ref_binds_ret_temp, -1,
  /* DiagArray769 */ diag::warn_for_range_const_ref_binds_temp_built_from_ref, diag::warn_for_range_copy, -1,
  /* DiagArray770 */ diag::warn_var_decl_not_read_only, -1,
  /* DiagArray771 */ diag::warn_auto_readonly_iboutlet_property, -1,
  /* DiagArray772 */ diag::warn_bad_receiver_type, -1,
  /* DiagArray773 */ diag::warn_receiver_forward_class, diag::warn_receiver_forward_instance, -1,
  /* DiagArray774 */ diag::ext_member_redeclared, -1,
  /* DiagArray775 */ diag::warn_consteval_if_always_true, -1,
  /* DiagArray777 */ diag::warn_redundant_move_on_return, -1,
  /* DiagArray778 */ diag::warn_redundant_parens_around_declarator, -1,
  /* DiagArray779 */ diag::ext_register_storage_class, -1,
  /* DiagArray780 */ diag::warn_reinterpret_different_from_static, -1,
  /* DiagArray781 */ diag::remark_fe_backend_plugin, -1,
  /* DiagArray783 */ diag::warn_initializer_out_of_order, diag::warn_some_initializers_out_of_order, -1,
  /* DiagArray784 */ diag::ext_designated_init_reordered, -1,
  /* DiagArray785 */ diag::warn_objc_requires_super_protocol, -1,
  /* DiagArray787 */ diag::warn_reserved_extern_symbol, -1,
  /* DiagArray788 */ diag::warn_pp_macro_is_reserved_id, -1,
  /* DiagArray789 */ diag::warn_reserved_module_name, -1,
  /* DiagArray790 */ diag::ext_ms_reserved_user_defined_literal, diag::ext_reserved_user_defined_literal, -1,
  /* DiagArray791 */ diag::warn_pragma_restrict_expansion_macro_use, -1,
  /* DiagArray792 */ diag::ext_retained_language_linkage, -1,
  /* DiagArray794 */ diag::ext_return_has_expr, diag::ext_return_missing_expr, diag::warn_return_missing_expr, -1,
  /* DiagArray795 */ diag::warn_ret_addr_label, diag::warn_ret_local_temp_addr_ref, diag::warn_ret_stack_addr_ref, -1,
  /* DiagArray797 */ diag::warn_falloff_nonvoid_coroutine, diag::warn_falloff_nonvoid_function, diag::warn_falloff_nonvoid_lambda, diag::warn_maybe_falloff_nonvoid_coroutine, diag::warn_maybe_falloff_nonvoid_function, diag::warn_maybe_falloff_nonvoid_lambda, -1,
  /* DiagArray798 */ diag::warn_return_value_udt, diag::warn_return_value_udt_incomplete, -1,
  /* DiagArray799 */ diag::ext_ovl_rewrite_equalequal_not_bool, -1,
  /* DiagArray800 */ diag::remark_cc1_round_trip_generated, -1,
  /* DiagArray801 */ diag::warn_no_dynamic_cast_with_rtti_disabled, diag::warn_no_typeid_with_rtti_disabled, -1,
  /* DiagArray802 */ diag::remark_sanitize_address_insert_extra_padding_accepted, diag::remark_sanitize_address_insert_extra_padding_rejected, -1,
  /* DiagArray803 */ diag::warn_drv_sarif_format_unstable, -1,
  /* DiagArray804 */ diag::remark_pp_search_path_usage, -1,
  /* DiagArray805 */ diag::warn_attribute_section_on_redeclaration, diag::warn_duplicate_codeseg_attribute, diag::warn_mismatched_section, -1,
  /* DiagArray806 */ diag::warn_unimplemented_selector, -1,
  /* DiagArray807 */ diag::warn_multiple_selectors, -1,
  /* DiagArray808 */ diag::warn_self_assignment_builtin, -1,
  /* DiagArray809 */ diag::warn_identity_field_assign, -1,
  /* DiagArray810 */ diag::warn_self_assignment_overloaded, -1,
  /* DiagArray811 */ diag::warn_self_move, -1,
  /* DiagArray812 */ diag::warn_semicolon_before_method_body, -1,
  /* DiagArray813 */ diag::warn_missing_sentinel, diag::warn_not_enough_argument, -1,
  /* DiagArray815 */ diag::warn_fe_serialized_diag_failure, diag::warn_fe_serialized_diag_failure_during_finalization, diag::warn_fe_serialized_diag_merge_failure, -1,
  /* DiagArray816 */ diag::warn_decl_shadow, -1,
  /* DiagArray818 */ diag::warn_shadow_field, -1,
  /* DiagArray819 */ diag::warn_ctor_parm_shadows_field, -1,
  /* DiagArray820 */ diag::warn_modifying_shadowing_decl, -1,
  /* DiagArray821 */ diag::warn_ivar_use_hidden, -1,
  /* DiagArray822 */ diag::warn_decl_shadow_uncaptured_local, -1,
  /* DiagArray823 */ diag::warn_shift_negative, -1,
  /* DiagArray824 */ diag::warn_shift_gt_typewidth, -1,
  /* DiagArray825 */ diag::warn_shift_lhs_negative, -1,
  /* DiagArray826 */ diag::warn_addition_in_bitshift, -1,
  /* DiagArray827 */ diag::warn_shift_result_gt_typewidth, -1,
  /* DiagArray828 */ diag::warn_shift_result_sets_sign_bit, -1,
  /* DiagArray829 */ diag::warn_impcast_integer_64_32, -1,
  /* DiagArray830 */ diag::warn_mixed_sign_comparison, -1,
  /* DiagArray831 */ diag::warn_impcast_integer_sign, diag::warn_impcast_integer_sign_conditional, diag::warn_impcast_nonnegative_result, -1,
  /* DiagArray833 */ diag::warn_no_underlying_type_specified_for_enum_bitfield, -1,
  /* DiagArray834 */ diag::ext_wchar_t_sign_spec, -1,
  /* DiagArray835 */ diag::warn_impcast_single_bit_bitield_precision_constant, -1,
  /* DiagArray836 */ diag::warn_sizeof_array_param, -1,
  /* DiagArray837 */ diag::warn_sizeof_array_decay, -1,
  /* DiagArray838 */ diag::warn_division_sizeof_array, -1,
  /* DiagArray839 */ diag::warn_division_sizeof_ptr, -1,
  /* DiagArray840 */ diag::warn_sizeof_pointer_expr_memaccess, diag::warn_sizeof_pointer_type_memaccess, -1,
  /* DiagArray841 */ diag::warn_slash_u_filename, -1,
  /* DiagArray842 */ diag::warn_slh_does_not_support_asm_goto, -1,
  /* DiagArray843 */ diag::remark_sloc_usage, -1,
  /* DiagArray844 */ diag::warn_sometimes_uninit_var, -1,
  /* DiagArray845 */ diag::warn_fe_source_mgr, -1,
  /* DiagArray846 */ diag::warn_acc_clause_unimplemented, diag::warn_acc_construct_unimplemented, diag::warn_pragma_acc_ignored, -1,
  /* DiagArray847 */ diag::warn_omp_declare_variant_after_emitted, diag::warn_omp_declare_variant_after_used, diag::warn_omp_declare_variant_marked_as_declare_variant, diag::warn_omp_declare_variant_score_not_constant, diag::warn_omp_nesting_simd, diag::warn_omp_unterminated_declare_target, diag::warn_pragma_omp_ignored, diag::warn_unknown_declare_variant_isa_trait, -1,
  /* DiagArray848 */ diag::warn_sampler_initializer_invalid_bits, -1,
  /* DiagArray850 */ diag::warn_stack_exhausted, -1,
  /* DiagArray851 */ diag::warn_stack_clash_protection_inline_asm, -1,
  /* DiagArray852 */ diag::ext_in_class_initializer_float_type_cxx11, -1,
  /* DiagArray853 */ diag::ext_internal_in_extern_inline, diag::ext_internal_in_extern_inline_quiet, -1,
  /* DiagArray854 */ diag::warn_static_inline_explicit_inst_ignored, -1,
  /* DiagArray855 */ diag::warn_static_local_in_extern_inline, -1,
  /* DiagArray856 */ diag::warn_static_self_reference_in_init, -1,
  /* DiagArray857 */ diag::warn_drv_libstdcxx_not_found, -1,
  /* DiagArray869 */ diag::warn_strict_potentially_direct_selector_expression, -1,
  /* DiagArray870 */ diag::ext_compat_template_param_shadow, -1,
  /* DiagArray871 */ diag::warn_strict_prototypes, -1,
  /* DiagArray872 */ diag::warn_strict_multiple_method_decl, -1,
  /* DiagArray873 */ diag::warn_stringcompare, -1,
  /* DiagArray874 */ diag::warn_concatenated_literal_array_init, -1,
  /* DiagArray875 */ diag::warn_impcast_string_literal_to_bool, -1,
  /* DiagArray876 */ diag::warn_string_plus_char, -1,
  /* DiagArray877 */ diag::warn_string_plus_int, -1,
  /* DiagArray878 */ diag::warn_strlcpycat_wrong_size, -1,
  /* DiagArray879 */ diag::warn_strncat_large_size, diag::warn_strncat_src_size, diag::warn_strncat_wrong_size, -1,
  /* DiagArray880 */ diag::warn_suggest_destructor_marked_not_override_overriding, -1,
  /* DiagArray881 */ diag::warn_suggest_function_marked_not_override_overriding, -1,
  /* DiagArray882 */ diag::ext_typecheck_base_super, -1,
  /* DiagArray883 */ diag::warn_suspicious_bzero_size, -1,
  /* DiagArray885 */ diag::warn_attr_swift_name_decl_kind, diag::warn_attr_swift_name_decl_missing_params, diag::warn_attr_swift_name_function, diag::warn_attr_swift_name_getter_parameters, diag::warn_attr_swift_name_invalid_identifier, diag::warn_attr_swift_name_missing_parameters, diag::warn_attr_swift_name_multiple_selfs, diag::warn_attr_swift_name_num_params, diag::warn_attr_swift_name_setter_parameters, diag::warn_attr_swift_name_subscript_getter_newValue, diag::warn_attr_swift_name_subscript_invalid_parameter, diag::warn_attr_swift_name_subscript_setter_multiple_newValues, diag::warn_attr_swift_name_subscript_setter_no_newValue, -1,
  /* DiagArray886 */ diag::warn_case_value_overflow, diag::warn_missing_case, diag::warn_not_in_enum, -1,
  /* DiagArray887 */ diag::warn_bool_switch_condition, -1,
  /* DiagArray888 */ diag::warn_switch_default, -1,
  /* DiagArray889 */ diag::warn_def_missing_case, -1,
  /* DiagArray890 */ diag::warn_sync_op_misaligned, -1,
  /* DiagArray891 */ diag::warn_sync_fetch_and_nand_semantics_change, -1,
  /* DiagArray893 */ diag::warn_target_clone_mixed_values, -1,
  /* DiagArray894 */ diag::warn_comparison_bitwise_always, diag::warn_comparison_bitwise_or, -1,
  /* DiagArray895 */ diag::warn_alignment_builtin_useless, diag::warn_comparison_always, -1,
  /* DiagArray896 */ diag::warn_integer_constants_in_conditional_always_true, diag::warn_left_shift_always, diag::warn_tautological_bool_compare, -1,
  /* DiagArray898 */ diag::warn_out_of_range_compare, -1,
  /* DiagArray899 */ diag::warn_tautological_negation_and_compare, diag::warn_tautological_negation_or_compare, -1,
  /* DiagArray900 */ diag::warn_tautological_compare_objc_bool, -1,
  /* DiagArray901 */ diag::warn_tautological_overlap_comparison, -1,
  /* DiagArray902 */ diag::warn_nonnull_expr_compare, diag::warn_null_pointer_compare, -1,
  /* DiagArray903 */ diag::warn_tautological_constant_compare, -1,
  /* DiagArray904 */ diag::warn_address_of_reference_null_compare, diag::warn_this_null_compare, -1,
  /* DiagArray905 */ diag::warn_unsigned_char_always_true_comparison, -1,
  /* DiagArray906 */ diag::warn_unsigned_enum_always_true_comparison, -1,
  /* DiagArray907 */ diag::warn_unsigned_always_true_comparison, -1,
  /* DiagArray908 */ diag::warn_tautological_compare_value_range, -1,
  /* DiagArray909 */ diag::warn_tcb_enforcement_violation, -1,
  /* DiagArray910 */ diag::ext_template_after_declarative_nns, -1,
  /* DiagArray911 */ diag::warn_tentative_incomplete_array, -1,
  /* DiagArray912 */ diag::ext_typecheck_decl_incomplete_type, -1,
  /* DiagArray914 */ diag::warn_acquired_before, diag::warn_acquired_before_after_cycle, diag::warn_cannot_resolve_lock, diag::warn_double_lock, diag::warn_expecting_lock_held_on_loop, diag::warn_expecting_locked, diag::warn_fun_excludes_mutex, diag::warn_fun_requires_lock, diag::warn_fun_requires_negative_cap, diag::warn_lock_exclusive_and_shared, diag::warn_lock_some_predecessors, diag::warn_no_unlock, diag::warn_unlock_but_no_lock, diag::warn_unlock_kind_mismatch, diag::warn_var_deref_requires_any_lock, diag::warn_var_deref_requires_lock, diag::warn_variable_requires_any_lock, diag::warn_variable_requires_lock, -1,
  /* DiagArray915 */ diag::warn_thread_attribute_argument_not_lockable, diag::warn_thread_attribute_decl_not_lockable, diag::warn_thread_attribute_decl_not_pointer, diag::warn_thread_attribute_ignored, diag::warn_thread_attribute_not_on_capability_member, diag::warn_thread_attribute_not_on_non_static_member, -1,
  /* DiagArray916 */ diag::warn_thread_safety_beta, -1,
  /* DiagArray917 */ diag::warn_acquire_requires_negative_cap, -1,
  /* DiagArray918 */ diag::warn_fun_requires_lock_precise, diag::warn_var_deref_requires_lock_precise, diag::warn_variable_requires_lock_precise, -1,
  /* DiagArray919 */ diag::warn_guarded_pass_by_reference, diag::warn_pt_guarded_pass_by_reference, -1,
  /* DiagArray920 */ diag::warn_guarded_return_by_reference, diag::warn_pt_guarded_return_by_reference, -1,
  /* DiagArray921 */ diag::warn_thread_safety_verbose, -1,
  /* DiagArray922 */ diag::trigraph_converted, diag::trigraph_ends_block_comment, diag::trigraph_ignored, diag::trigraph_ignored_block_comment, -1,
  /* DiagArray924 */ diag::warn_type_safety_null_pointer_required, diag::warn_type_safety_type_mismatch, diag::warn_type_tag_for_datatype_wrong_kind, -1,
  /* DiagArray925 */ diag::ext_redefinition_of_typedef, -1,
  /* DiagArray926 */ diag::ext_typename_missing, -1,
  /* DiagArray927 */ diag::warn_fe_unable_to_open_stats_file, -1,
  /* DiagArray928 */ diag::warn_unaligned_access, -1,
  /* DiagArray929 */ diag::warn_imp_cast_drops_unaligned, -1,
  /* DiagArray930 */ diag::warn_unavailable_fwdclass_message, -1,
  /* DiagArray931 */ diag::warn_undeclared_selector, diag::warn_undeclared_selector_with_typo, -1,
  /* DiagArray932 */ diag::warn_pp_undef_identifier, -1,
  /* DiagArray933 */ diag::warn_pp_undef_prefix, -1,
  /* DiagArray934 */ diag::warn_attribute_arm_sm_incompat_builtin, -1,
  /* DiagArray935 */ diag::warn_attribute_arm_za_builtin_no_za_state, -1,
  /* DiagArray936 */ diag::warn_attribute_arm_zt0_builtin_no_zt0_state, -1,
  /* DiagArray937 */ diag::warn_address_of_reference_bool_conversion, diag::warn_this_bool_conversion, -1,
  /* DiagArray938 */ diag::warn_func_template_missing, -1,
  /* DiagArray939 */ diag::warn_undefined_inline, -1,
  /* DiagArray940 */ diag::warn_undefined_internal, -1,
  /* DiagArray941 */ diag::ext_undefined_internal_type, -1,
  /* DiagArray942 */ diag::warn_pointer_indirection_from_incompatible_type, diag::warn_undefined_reinterpret_cast, -1,
  /* DiagArray943 */ diag::warn_var_template_missing, -1,
  /* DiagArray944 */ diag::warn_throw_underaligned_obj, -1,
  /* DiagArray945 */ diag::warn_side_effects_unevaluated_context, -1,
  /* DiagArray946 */ diag::warn_unguarded_availability, diag::warn_unguarded_availability_unavailable, -1,
  /* DiagArray947 */ diag::warn_unguarded_availability_new, diag::warn_unguarded_availability_unavailable_new, -1,
  /* DiagArray948 */ diag::warn_delimited_ucn_empty, diag::warn_delimited_ucn_incomplete, diag::warn_ucn_escape_incomplete, diag::warn_ucn_escape_no_digits, diag::warn_ucn_escape_surrogate, diag::warn_ucn_not_valid_in_c89, diag::warn_ucn_not_valid_in_c89_literal, -1,
  /* DiagArray949 */ diag::warn_utf8_symbol_homoglyph, -1,
  /* DiagArray950 */ diag::ext_unicode_whitespace, -1,
  /* DiagArray951 */ diag::warn_utf8_symbol_zero_width, -1,
  /* DiagArray952 */ diag::warn_base_class_is_uninit, diag::warn_field_is_uninit, diag::warn_reference_field_is_uninit, diag::warn_uninit_byref_blockvar_captured_by_block, diag::warn_uninit_self_reference_in_init, diag::warn_uninit_self_reference_in_reference_init, diag::warn_uninit_var, -1,
  /* DiagArray953 */ diag::warn_uninit_const_reference, -1,
  /* DiagArray954 */ diag::warn_drv_potentially_misspelled_joined_argument, diag::warn_drv_unknown_argument_clang_cl, diag::warn_drv_unknown_argument_clang_cl_with_suggestion, -1,
  /* DiagArray955 */ diag::warn_omp_assume_attribute_string_unknown, -1,
  /* DiagArray956 */ diag::warn_unknown_attribute_ignored, -1,
  /* DiagArray957 */ diag::warn_drv_new_cuda_version, diag::warn_drv_partially_supported_cuda_version, -1,
  /* DiagArray958 */ diag::warn_pp_invalid_directive, -1,
  /* DiagArray959 */ diag::ext_unknown_escape, -1,
  /* DiagArray960 */ diag::ext_on_off_switch_syntax, diag::ext_pragma_syntax_eod, diag::ext_stdc_pragma_ignored, diag::warn_pragma_diagnostic_cannot_pop, diag::warn_pragma_diagnostic_invalid, diag::warn_pragma_diagnostic_invalid_option, diag::warn_pragma_diagnostic_invalid_token, diag::warn_pragma_exec_charset_expected, diag::warn_pragma_exec_charset_push_invalid, diag::warn_pragma_exec_charset_spec_invalid, diag::warn_pragma_ignored, diag::warn_pragma_include_alias_expected, diag::warn_pragma_include_alias_expected_filename, diag::warn_pragma_include_alias_mismatch_angle, diag::warn_pragma_include_alias_mismatch_quote, diag::warn_pragma_warning_expected, diag::warn_pragma_warning_expected_number, diag::warn_pragma_warning_push_level, diag::warn_pragma_warning_spec_invalid, diag::warn_stdc_fenv_round_not_supported, -1,
  /* DiagArray961 */ diag::warn_unknown_sanitizer_ignored, -1,
  /* DiagArray962 */ diag::warn_pragma_diagnostic_unknown_warning, diag::warn_unknown_diag_option, diag::warn_unknown_warning_specifier, -1,
  /* DiagArray963 */ diag::ext_template_arg_unnamed_type, -1,
  /* DiagArray964 */ diag::warn_unneeded_internal_decl, diag::warn_unneeded_static_internal_decl, -1,
  /* DiagArray965 */ diag::warn_unneeded_member_function, -1,
  /* DiagArray966 */ diag::warn_unqualified_call_to_std_cast_function, -1,
  /* DiagArray967 */ diag::warn_unreachable, -1,
  /* DiagArray969 */ diag::warn_unreachable_break, -1,
  /* DiagArray970 */ diag::warn_unreachable_fallthrough_attr, -1,
  /* DiagArray971 */ diag::warn_unreachable_association, -1,
  /* DiagArray972 */ diag::warn_unreachable_loop_increment, -1,
  /* DiagArray973 */ diag::warn_unreachable_return, -1,
  /* DiagArray974 */ diag::warn_unsafe_buffer_operation, diag::warn_unsafe_buffer_variable, -1,
  /* DiagArray975 */ diag::warn_unsafe_buffer_usage_in_container, -1,
  /* DiagArray976 */ diag::warn_unsequenced_mod_mod, diag::warn_unsequenced_mod_use, -1,
  /* DiagArray977 */ diag::warn_drv_no_floating_point_registers, diag::warn_drv_unsupported_float_abi_by_lib, -1,
  /* DiagArray978 */ diag::warn_target_unsupported_abs2008, diag::warn_target_unsupported_abslegacy, -1,
  /* DiagArray979 */ diag::warn_at_available_unchecked_use, -1,
  /* DiagArray980 */ diag::warn_target_unsupported_compact_branches, -1,
  /* DiagArray981 */ diag::warn_attribute_dll_instantiated_base_class, -1,
  /* DiagArray982 */ diag::warn_fe_backend_unsupported_fp_exceptions, diag::warn_fe_backend_unsupported_fp_rounding, -1,
  /* DiagArray983 */ diag::warn_template_qualified_friend_ignored, diag::warn_template_qualified_friend_unsupported, -1,
  /* DiagArray984 */ diag::warn_drv_unsupported_gpopt, -1,
  /* DiagArray985 */ diag::warn_target_unsupported_nan2008, diag::warn_target_unsupported_nanlegacy, -1,
  /* DiagArray986 */ diag::warn_drv_dwarf_version_limited_by_target, diag::warn_drv_unsupported_debug_info_opt_for_target, -1,
  /* DiagArray987 */ diag::warn_attribute_protected_visibility, -1,
  /* DiagArray988 */ diag::ext_partial_specs_not_deducible, -1,
  /* DiagArray991 */ diag::warn_unused_but_set_parameter, -1,
  /* DiagArray992 */ diag::warn_unused_but_set_variable, -1,
  /* DiagArray993 */ diag::warn_drv_diagnostics_hotness_requires_pgo, diag::warn_drv_diagnostics_misexpect_requires_pgo, diag::warn_drv_empty_joined_argument, diag::warn_drv_input_file_unused, diag::warn_drv_input_file_unused_by_cpp, diag::warn_drv_invalid_arch_name_with_suggestion, diag::warn_drv_large_data_threshold_invalid_code_model, diag::warn_drv_preprocessed_input_file_unused, diag::warn_drv_unused_argument, diag::warn_drv_unused_x, diag::warn_ignored_clang_option, diag::warn_ignoring_fdiscard_for_bitcode, diag::warn_ignoring_verify_debuginfo_preserve_export, -1,
  /* DiagArray994 */ diag::warn_unused_comparison, -1,
  /* DiagArray995 */ diag::warn_unused_const_variable, -1,
  /* DiagArray996 */ diag::warn_unused_exception_param, -1,
  /* DiagArray997 */ diag::warn_unused_function, -1,
  /* DiagArray998 */ diag::warn_unused_property_expr, -1,
  /* DiagArray999 */ diag::warn_unused_label, -1,
  /* DiagArray1000 */ diag::warn_unused_lambda_capture, -1,
  /* DiagArray1001 */ diag::warn_unused_local_typedef, -1,
  /* DiagArray1003 */ diag::pp_macro_not_used, -1,
  /* DiagArray1004 */ diag::warn_unused_member_function, -1,
  /* DiagArray1005 */ diag::warn_unused_parameter, -1,
  /* DiagArray1006 */ diag::warn_unused_private_field, -1,
  /* DiagArray1007 */ diag::warn_unused_property_backing_ivar, -1,
  /* DiagArray1008 */ diag::warn_unused_result, diag::warn_unused_result_msg, -1,
  /* DiagArray1009 */ diag::warn_unused_template, -1,
  /* DiagArray1010 */ diag::warn_unused_call, diag::warn_unused_comma_left_operand, diag::warn_unused_constructor, diag::warn_unused_constructor_msg, diag::warn_unused_container_subscript_expr, diag::warn_unused_expr, diag::warn_unused_voidptr, -1,
  /* DiagArray1011 */ diag::warn_unused_variable, -1,
  /* DiagArray1012 */ diag::warn_unused_volatile, -1,
  /* DiagArray1013 */ diag::warn_used_but_marked_unused, -1,
  /* DiagArray1014 */ diag::warn_user_literal_reserved, -1,
  /* DiagArray1015 */ diag::warn_diagnose_if_succeeded, -1,
  /* DiagArray1016 */ diag::warn_second_arg_of_va_start_not_last_named_param, diag::warn_second_parameter_to_va_arg_never_compatible, diag::warn_va_start_type_is_undefined, -1,
  /* DiagArray1017 */ diag::ext_named_variadic_macro, diag::ext_pp_bad_vaopt_use, diag::ext_variadic_macro, -1,
  /* DiagArray1018 */ diag::warn_typecheck_vector_element_sizes_not_equal, -1,
  /* DiagArray1019 */ diag::warn_incompatible_vectors, -1,
  /* DiagArray1021 */ diag::warn_empty_parens_are_function_decl, diag::warn_parens_disambiguated_as_function_declaration, diag::warn_parens_disambiguated_as_variable_declaration, -1,
  /* DiagArray1022 */ diag::warn_decl_in_param_list, diag::warn_redefinition_in_param_list, -1,
  /* DiagArray1023 */ diag::warn_vla_used, -1,
  /* DiagArray1024 */ diag::ext_vla_cxx, diag::ext_vla_cxx_in_gnu_mode, -1,
  /* DiagArray1025 */ diag::ext_vla, -1,
  /* DiagArray1026 */ diag::ext_vla_cxx_in_gnu_mode_static_assert, diag::ext_vla_cxx_static_assert, -1,
  /* DiagArray1027 */ diag::warn_void_pointer_to_enum_cast, -1,
  /* DiagArray1028 */ diag::warn_void_pointer_to_int_cast, -1,
  /* DiagArray1029 */ diag::ext_typecheck_indirection_through_void_pointer, -1,
  /* DiagArray1031 */ diag::warn_wasm_dynamic_exception_spec_ignored, -1,
  /* DiagArray1032 */ diag::warn_weak_template_vtable, -1,
  /* DiagArray1033 */ diag::warn_weak_vtable, -1,
  /* DiagArray1034 */ diag::ext_deprecated_string_literal_conversion, -1,
  /* DiagArray1036 */ diag::warn_xor_used_as_pow, diag::warn_xor_used_as_pow_base, diag::warn_xor_used_as_pow_base_extra, -1,
  /* DiagArray1037 */ diag::warn_zero_as_null_pointer_constant, -1,
  /* DiagArray1038 */ diag::ext_typecheck_zero_array_size, -1,
};

static const int16_t DiagSubGroups[] = {
  /* Empty */ -1,
  /* DiagSubGroup0 */ 314, -1,
  /* DiagSubGroup4 */ 19, 314, -1,
  /* DiagSubGroup12 */ 717, 873, 902, -1,
  /* DiagSubGroup19 */ 585, 703, 886, 887, 551, 700, 1024, -1,
  /* DiagSubGroup30 */ 224, -1,
  /* DiagSubGroup33 */ 40, 39, 36, -1,
  /* DiagSubGroup38 */ 35, -1,
  /* DiagSubGroup46 */ 47, -1,
  /* DiagSubGroup56 */ 416, 201, -1,
  /* DiagSubGroup60 */ 956, 406, -1,
  /* DiagSubGroup71 */ 108, 140, -1,
  /* DiagSubGroup72 */ 143, -1,
  /* DiagSubGroup75 */ 835, -1,
  /* DiagSubGroup82 */ 717, 937, -1,
  /* DiagSubGroup83 */ 82, -1,
  /* DiagSubGroup84 */ 79, -1,
  /* DiagSubGroup94 */ 97, -1,
  /* DiagSubGroup95 */ 101, -1,
  /* DiagSubGroup96 */ 105, -1,
  /* DiagSubGroup97 */ 105, 100, 98, 733, 735, 737, 739, -1,
  /* DiagSubGroup99 */ 97, 734, 736, 738, 740, -1,
  /* DiagSubGroup101 */ 102, 103, 104, -1,
  /* DiagSubGroup105 */ 106, -1,
  /* DiagSubGroup109 */ 735, 737, 739, -1,
  /* DiagSubGroup110 */ 109, 736, 738, 740, -1,
  /* DiagSubGroup111 */ 108, 107, -1,
  /* DiagSubGroup113 */ 252, 242, 114, 737, 739, -1,
  /* DiagSubGroup115 */ 113, 738, 740, -1,
  /* DiagSubGroup116 */ 112, -1,
  /* DiagSubGroup117 */ 111, -1,
  /* DiagSubGroup118 */ 113, -1,
  /* DiagSubGroup119 */ 114, -1,
  /* DiagSubGroup120 */ 116, -1,
  /* DiagSubGroup122 */ 739, -1,
  /* DiagSubGroup123 */ 122, 740, -1,
  /* DiagSubGroup125 */ 124, 121, -1,
  /* DiagSubGroup127 */ 128, -1,
  /* DiagSubGroup130 */ 122, -1,
  /* DiagSubGroup131 */ 123, -1,
  /* DiagSubGroup132 */ 125, -1,
  /* DiagSubGroup133 */ 127, -1,
  /* DiagSubGroup134 */ 129, -1,
  /* DiagSubGroup135 */ 737, -1,
  /* DiagSubGroup136 */ 738, -1,
  /* DiagSubGroup137 */ 735, -1,
  /* DiagSubGroup138 */ 736, -1,
  /* DiagSubGroup139 */ 733, -1,
  /* DiagSubGroup141 */ 734, -1,
  /* DiagSubGroup142 */ 145, 147, 733, 735, 737, 739, -1,
  /* DiagSubGroup146 */ 142, 143, 144, 734, 736, 738, 740, -1,
  /* DiagSubGroup151 */ 149, -1,
  /* DiagSubGroup152 */ 150, -1,
  /* DiagSubGroup154 */ 124, -1,
  /* DiagSubGroup155 */ 154, -1,
  /* DiagSubGroup157 */ 176, -1,
  /* DiagSubGroup160 */ 162, 161, -1,
  /* DiagSubGroup170 */ 605, -1,
  /* DiagSubGroup174 */ 173, -1,
  /* DiagSubGroup178 */ 179, 180, -1,
  /* DiagSubGroup184 */ 75, 632, -1,
  /* DiagSubGroup189 */ 82, 184, 299, 76, 326, 465, 426, 423, 497, 602, 620, 643, 831, 875, -1,
  /* DiagSubGroup190 */ 620, -1,
  /* DiagSubGroup192 */ 193, 234, 22, 191, -1,
  /* DiagSubGroup195 */ 2, -1,
  /* DiagSubGroup203 */ 205, 207, 206, 795, -1,
  /* DiagSubGroup219 */ 218, 216, -1,
  /* DiagSubGroup222 */ 224, 225, 226, 228, 229, 231, 235, 236, 237, 238, 239, 240, 227, 242, 243, 250, 252, 245, 253, 254, 255, 256, 251, -1,
  /* DiagSubGroup229 */ 232, -1,
  /* DiagSubGroup230 */ 231, -1,
  /* DiagSubGroup231 */ 233, -1,
  /* DiagSubGroup248 */ 249, -1,
  /* DiagSubGroup256 */ 98, -1,
  /* DiagSubGroup262 */ 263, -1,
  /* DiagSubGroup267 */ 269, 268, -1,
  /* DiagSubGroup270 */ 271, -1,
  /* DiagSubGroup283 */ 236, -1,
  /* DiagSubGroup285 */ 607, -1,
  /* DiagSubGroup294 */ 318, -1,
  /* DiagSubGroup295 */ 297, 237, -1,
  /* DiagSubGroup296 */ 238, -1,
  /* DiagSubGroup299 */ 300, 301, 296, -1,
  /* DiagSubGroup300 */ 239, -1,
  /* DiagSubGroup301 */ 240, -1,
  /* DiagSubGroup314 */ 229, 561, 413, 457, 812, 564, 830, 1005, 991, 622, 623, 291, 874, 358, 161, -1,
  /* DiagSubGroup316 */ 144, 102, -1,
  /* DiagSubGroup317 */ 291, -1,
  /* DiagSubGroup326 */ 328, 329, -1,
  /* DiagSubGroup331 */ 332, 346, 608, 340, 345, 334, 333, 337, 342, -1,
  /* DiagSubGroup337 */ 338, -1,
  /* DiagSubGroup342 */ 343, -1,
  /* DiagSubGroup347 */ 336, 340, 345, -1,
  /* DiagSubGroup348 */ 337, 342, -1,
  /* DiagSubGroup352 */ 351, -1,
  /* DiagSubGroup357 */ 893, -1,
  /* DiagSubGroup359 */ 107, 112, 121, 126, -1,
  /* DiagSubGroup365 */ 366, 367, 369, 370, 371, 372, 373, 374, 375, 377, 1025, 378, 379, 380, 381, 382, 384, 385, 386, 387, 388, 774, 389, 390, 392, 393, 394, 395, 1038, 396, 397, -1,
  /* DiagSubGroup390 */ 391, -1,
  /* DiagSubGroup412 */ 410, 411, -1,
  /* DiagSubGroup413 */ 414, -1,
  /* DiagSubGroup415 */ 424, 425, -1,
  /* DiagSubGroup420 */ 421, -1,
  /* DiagSubGroup423 */ 427, 666, -1,
  /* DiagSubGroup426 */ 829, 667, -1,
  /* DiagSubGroup427 */ 417, -1,
  /* DiagSubGroup442 */ 443, 437, -1,
  /* DiagSubGroup448 */ 450, 604, -1,
  /* DiagSubGroup454 */ 242, -1,
  /* DiagSubGroup466 */ 465, -1,
  /* DiagSubGroup468 */ 469, -1,
  /* DiagSubGroup471 */ 409, -1,
  /* DiagSubGroup499 */ 145, -1,
  /* DiagSubGroup502 */ 104, -1,
  /* DiagSubGroup503 */ 330, 767, -1,
  /* DiagSubGroup515 */ 519, 524, 536, 522, 532, 541, 516, 547, 528, 548, 538, 539, 546, 530, 527, 523, 544, 540, 526, 534, 533, 531, 518, 521, 549, 517, 520, 525, 542, 537, 543, 451, -1,
  /* DiagSubGroup544 */ 545, -1,
  /* DiagSubGroup561 */ 559, -1,
  /* DiagSubGroup585 */ 45, 84, 167, 173, 219, 331, 330, 350, 415, 455, 467, 555, 556, 586, 590, 769, 782, 797, 808, 811, 836, 837, 877, 895, 922, 952, 960, 989, 1030, 648, 636, 639, 694, 753, 163, 312, 1015, -1,
  /* DiagSubGroup586 */ 715, 777, 796, 811, -1,
  /* DiagSubGroup587 */ 536, -1,
  /* DiagSubGroup593 */ 105, -1,
  /* DiagSubGroup599 */ 114, -1,
  /* DiagSubGroup601 */ 830, 189, 498, -1,
  /* DiagSubGroup604 */ 603, -1,
  /* DiagSubGroup622 */ 386, -1,
  /* DiagSubGroup625 */ 626, -1,
  /* DiagSubGroup635 */ 662, -1,
  /* DiagSubGroup642 */ 668, -1,
  /* DiagSubGroup662 */ 663, -1,
  /* DiagSubGroup665 */ 667, 666, 632, 900, -1,
  /* DiagSubGroup674 */ 846, -1,
  /* DiagSubGroup678 */ 847, 680, 682, 684, 683, 679, 681, 685, -1,
  /* DiagSubGroup684 */ 683, -1,
  /* DiagSubGroup695 */ 457, -1,
  /* DiagSubGroup699 */ 700, -1,
  /* DiagSubGroup701 */ 702, -1,
  /* DiagSubGroup703 */ 501, 500, 78, 80, 826, 693, 704, 204, -1,
  /* DiagSubGroup705 */ 946, -1,
  /* DiagSubGroup712 */ 74, 148, 102, 104, 107, 108, 112, 121, 124, 325, 359, 367, 369, 371, 372, 373, 374, 377, 378, 379, 381, 382, 384, 385, 386, 388, 389, 390, 394, 396, 397, 491, 502, 519, 520, 522, 525, 527, 532, 533, 540, 692, 1038, 272, 493, 488, 220, 431, 288, 292, 628, 322, 362, 61, 941, 792, 177, 594, -1,
  /* DiagSubGroup714 */ 250, 504, 90, 791, 321, -1,
  /* DiagSubGroup716 */ 388, -1,
  /* DiagSubGroup721 */ 1027, -1,
  /* DiagSubGroup722 */ 721, 1028, -1,
  /* DiagSubGroup729 */ 730, -1,
  /* DiagSubGroup732 */ 960, 412, 727, 729, -1,
  /* DiagSubGroup734 */ 733, 140, -1,
  /* DiagSubGroup736 */ 735, -1,
  /* DiagSubGroup738 */ 737, -1,
  /* DiagSubGroup740 */ 739, -1,
  /* DiagSubGroup742 */ 741, -1,
  /* DiagSubGroup743 */ 741, -1,
  /* DiagSubGroup744 */ 742, -1,
  /* DiagSubGroup746 */ 745, -1,
  /* DiagSubGroup748 */ 747, -1,
  /* DiagSubGroup749 */ 747, -1,
  /* DiagSubGroup750 */ 748, -1,
  /* DiagSubGroup767 */ 769, 768, -1,
  /* DiagSubGroup779 */ 252, -1,
  /* DiagSubGroup782 */ 783, 784, -1,
  /* DiagSubGroup786 */ 788, -1,
  /* DiagSubGroup787 */ 788, 789, 1014, -1,
  /* DiagSubGroup790 */ 100, -1,
  /* DiagSubGroup793 */ 795, -1,
  /* DiagSubGroup797 */ 798, 794, -1,
  /* DiagSubGroup806 */ 807, -1,
  /* DiagSubGroup808 */ 810, 809, -1,
  /* DiagSubGroup814 */ 976, -1,
  /* DiagSubGroup816 */ 820, 821, -1,
  /* DiagSubGroup817 */ 816, 819, 822, 818, -1,
  /* DiagSubGroup819 */ 820, -1,
  /* DiagSubGroup849 */ 848, -1,
  /* DiagSubGroup852 */ 392, -1,
  /* DiagSubGroup869 */ 725, -1,
  /* DiagSubGroup871 */ 246, -1,
  /* DiagSubGroup884 */ 840, 282, 615, 512, 883, -1,
  /* DiagSubGroup895 */ 896, 902, 901, 894, 904, 900, 899, -1,
  /* DiagSubGroup896 */ 898, -1,
  /* DiagSubGroup897 */ 923, 908, -1,
  /* DiagSubGroup913 */ 915, 914, 918, 919, -1,
  /* DiagSubGroup919 */ 920, -1,
  /* DiagSubGroup923 */ 903, 907, 905, 906, -1,
  /* DiagSubGroup945 */ 726, -1,
  /* DiagSubGroup946 */ 947, -1,
  /* DiagSubGroup952 */ 844, 856, 953, -1,
  /* DiagSubGroup963 */ 147, -1,
  /* DiagSubGroup967 */ 972, 970, 971, -1,
  /* DiagSubGroup968 */ 967, 969, 973, -1,
  /* DiagSubGroup974 */ 975, -1,
  /* DiagSubGroup989 */ 990, 997, 999, 1006, 1000, 1001, 1010, 1011, 992, 1007, -1,
  /* DiagSubGroup997 */ 964, -1,
  /* DiagSubGroup1002 */ 1001, -1,
  /* DiagSubGroup1004 */ 965, -1,
  /* DiagSubGroup1009 */ 964, -1,
  /* DiagSubGroup1010 */ 994, 1008, 945, -1,
  /* DiagSubGroup1011 */ 995, -1,
  /* DiagSubGroup1020 */ 1019, -1,
  /* DiagSubGroup1023 */ 1025, -1,
  /* DiagSubGroup1024 */ 1026, -1,
  /* DiagSubGroup1025 */ 1024, -1,
  /* DiagSubGroup1028 */ 1027, -1,
  /* DiagSubGroup1034 */ 256, -1,
  /* DiagSubGroup1035 */ 1034, -1,
};

static const char DiagGroupNames[] = {
    "\000\020#pragma-messages\t#warnings\020CFString-literal\003CL4\032Indep"
    "endentClass-attribute\022NSObject-attribute\026aarch64-sme-attributes\003"
    "abi\016absolute-value\024abstract-final-class\023abstract-vbase-init\007"
    "address\030address-of-packed-member\024address-of-temporary\020aggregat"
    "e-return\naix-compat\"alias-template-in-declaration-name\016align-misma"
    "tch\003all\006alloca\031alloca-with-align-alignof\027always-inline-coro"
    "utine\020ambiguous-delete\022ambiguous-ellipsis\017ambiguous-macro\031a"
    "mbiguous-member-template\033ambiguous-reversed-operator\034analyzer-inc"
    "ompatible-plugin\034android-unversioned-fallback\031anon-enum-enum-conv"
    "ersion\025anonymous-pack-parens\010apinotes\003arc%arc-bridge-casts-dis"
    "allowed-in-nonarc\036arc-maybe-repeated-use-of-weak\025arc-non-pod-mema"
    "ccess\031arc-performSelector-leaks\030arc-repeated-use-of-weak\021arc-r"
    "etain-cycles\032arc-unsafe-retained-assign\026argument-outside-range\034"
    "argument-undefined-behaviour\014array-bounds\037array-bounds-pointer-ar"
    "ithmetic\017array-parameter\003asm\022asm-operand-widths\013assign-enum"
    "\006assume\013at-protocol\034atimport-in-framework-header\015atomic-acc"
    "ess\020atomic-alignment\027atomic-implicit-seq-cst\026atomic-memory-ord"
    "ering\021atomic-properties*atomic-property-with-user-defined-accessor\035"
    "attribute-packed-for-bitfield\021attribute-warning\nattributes\024auto-"
    "decl-extensions\033auto-disable-vptr-sanitizer\013auto-import\022auto-s"
    "torage-class\013auto-var-id\014availability\030avr-rtlib-linking-quirks"
    "\016backend-plugin\030backslash-newline-escape\021bad-function-cast\016"
    "binary-literal\026bind-to-temporary-copy\024binding-in-condition\021bit"
    "-int-extension\034bitfield-constant-conversion\030bitfield-enum-convers"
    "ion\016bitfield-width\037bitwise-conditional-parentheses\032bitwise-ins"
    "tead-of-logical\026bitwise-op-parentheses\033block-capture-autoreleasin"
    "g\017bool-conversion\020bool-conversions\016bool-operation.bounds-safet"
    "y-counted-by-elt-type-unknown-size\022braced-scalar-init\021branch-prot"
    "ection\013bridge-cast builtin-assume-aligned-alignment\027builtin-macro"
    "-redefined\027builtin-memcpy-chk-size\027builtin-requires-header\nc++-c"
    "ompat\014c++0x-compat\020c++0x-extensions\017c++0x-narrowing\014c++11-c"
    "ompat(c++11-compat-deprecated-writable-strings\025c++11-compat-pedantic"
    "*c++11-compat-reserved-user-defined-literal\020c++11-extensions\020c++1"
    "1-extra-semi\026c++11-inline-namespace\017c++11-long-long\017c++11-narr"
    "owing\037c++11-narrowing-const-reference\032c++14-attribute-extensions\024"
    "c++14-binary-literal\014c++14-compat\025c++14-compat-pedantic\020c++14-"
    "extensions\032c++17-attribute-extensions\014c++17-compat\025c++17-compa"
    "t-mangling\025c++17-compat-pedantic\020c++17-extensions\020c++1y-extens"
    "ions\014c++1z-compat\025c++1z-compat-mangling\020c++1z-extensions\032c+"
    "+20-attribute-extensions\014c++20-compat\025c++20-compat-pedantic\020c+"
    "+20-designator\020c++20-extensions\032c++23-attribute-extensions\020c++"
    "23-extensions\027c++23-lambda-attributes\020c++26-extensions\014c++2a-c"
    "ompat\025c++2a-compat-pedantic\020c++2a-extensions\020c++2b-extensions\020"
    "c++2c-extensions\036c++98-c++11-c++14-c++17-compat'c++98-c++11-c++14-c+"
    "+17-compat-pedantic\030c++98-c++11-c++14-compat!c++98-c++11-c++14-compa"
    "t-pedantic\022c++98-c++11-compat!c++98-c++11-compat-binary-literal\033c"
    "++98-c++11-compat-pedantic\014c++98-compat#c++98-compat-bind-to-tempora"
    "ry-copy\027c++98-compat-extra-semi%c++98-compat-local-type-template-arg"
    "s\025c++98-compat-pedantic'c++98-compat-unnamed-type-template-args\016c"
    "11-extensions\nc23-compat\016c23-extensions\nc2x-compat\016c2x-extensio"
    "ns\nc99-compat\016c99-designator\016c99-extensions#call-to-pure-virtual"
    "-from-ctor-dtor\025called-once-parameter\ncast-align\027cast-calling-co"
    "nvention\022cast-function-type\033cast-function-type-mismatch\031cast-f"
    "unction-type-strict\020cast-of-sel-type\tcast-qual\023cast-qual-unrelat"
    "ed\nchar-align\017char-subscripts\014clang-cl-pch\020class-conversion\015"
    "class-varargs\017cmse-union-leak\005comma\007comment\010comments\036com"
    "pare-distinct-pointer-types\022completion-handler\026complex-component-"
    "init\024compound-token-split\035compound-token-split-by-macro\035compou"
    "nd-token-split-by-space\031conditional-type-mismatch\031conditional-uni"
    "nitialized\015config-macros\023constant-conversion\022constant-evaluate"
    "d\030constant-logical-operand\023constexpr-not-const\010consumed\nconve"
    "rsion\017conversion-null$coro-non-aligned-allocation-function\tcoroutin"
    "e%coroutine-missing-unhandled-exception\026covered-switch-default\003cp"
    "p\030cstring-format-directive\026ctad-maybe-unsupported\021ctor-dtor-pr"
    "ivacy\003ctu\013cuda-compat\030custom-atomic-properties\027cxx-attribut"
    "e-extension\010dangling\015dangling-else\016dangling-field\014dangling-"
    "gsl\031dangling-initializer-list\023darwin-sdk-settings\tdate-time\023d"
    "ealloc-in-category\035debug-compression-unavailable\033declaration-afte"
    "r-statement\032defaulted-function-deleted!delayed-template-parsing-in-c"
    "xx20\026delegating-ctor-cycles delete-abstract-non-virtual-dtor\021dele"
    "te-incomplete$delete-non-abstract-non-virtual-dtor\027delete-non-virtua"
    "l-dtor#delimited-escape-sequence-extension\032deprecate-lax-vec-conv-al"
    "l\ndeprecated\035deprecated-altivec-src-compat$deprecated-anon-enum-enu"
    "m-conversion\030deprecated-array-compare\025deprecated-attributes\023de"
    "precated-builtins\032deprecated-comma-subscript\017deprecated-copy\024d"
    "eprecated-copy-dtor\031deprecated-copy-with-dtor'deprecated-copy-with-u"
    "ser-provided-copy'deprecated-copy-with-user-provided-dtor\024deprecated"
    "-coroutine\027deprecated-declarations!deprecated-dynamic-exception-spec"
    "\027deprecated-enum-compare#deprecated-enum-compare-conditional\037depr"
    "ecated-enum-enum-conversion deprecated-enum-float-conversion\032depreca"
    "ted-implementations\031deprecated-increment-bool\033deprecated-literal-"
    "operator\031deprecated-module-dot-map,deprecated-no-relaxed-template-te"
    "mplate-args\030deprecated-non-prototype\031deprecated-objc-isa-usage%de"
    "precated-objc-pointer-introspection5deprecated-objc-pointer-introspecti"
    "on-performSelector\021deprecated-pragma)deprecated-redundant-constexpr-"
    "static-def\023deprecated-register\027deprecated-this-capture\017depreca"
    "ted-type\023deprecated-volatile\033deprecated-writable-strings\022direc"
    "t-ivar-access\030disabled-macro-expansion\025disabled-optimization\014d"
    "iscard-qual\034distributed-object-modifiers\013div-by-zero\020division-"
    "by-zero\036dll-attribute-on-redeclaration%dllexport-explicit-instantiat"
    "ion-decl\032dllimport-static-field-def\015documentation\035documentatio"
    "n-deprecated-sync\022documentation-html\026documentation-pedantic\035do"
    "cumentation-unknown-command\036dollar-in-identifier-extension\020double"
    "-promotion\tdtor-name\014dtor-typedef\030duplicate-decl-specifier\016du"
    "plicate-enum\024duplicate-method-arg\026duplicate-method-match\022dupli"
    "cate-protocol\017dxil-validation\027dynamic-class-memaccess\026dynamic-"
    "exception-spec\034eager-load-cxx-named-modules\006effc++\024elaborated-"
    "enum-base\025elaborated-enum-class\022embedded-directive\nempty-body\023"
    "empty-decomposition\017empty-init-stmt\026empty-translation-unit\013enc"
    "ode-type\014endif-labels\014enum-compare\030enum-compare-conditional\023"
    "enum-compare-switch\031enum-constexpr-conversion\017enum-conversion\024"
    "enum-enum-conversion\025enum-float-conversion\016enum-too-large\nexcept"
    "ions\023excess-initializers\021excessive-regsave\025exit-time-destructo"
    "rs\024expansion-to-defined\031experimental-header-units\030explicit-ini"
    "tialize-call\027explicit-ownership-type\016export-unnamed\017extern-c-c"
    "ompat\022extern-initializer\005extra\023extra-qualification\nextra-semi"
    "\017extra-semi-stmt\014extra-tokens\021extractapi-misuse\032final-dtor-"
    "non-final-class\013final-macro\024fixed-enum-extension\024fixed-point-o"
    "verflow\tflag-enum\031flexible-array-extensions\020float-conversion\013"
    "float-equal\031float-overflow-conversion\025float-zero-conversion\021fo"
    "r-loop-analysis\006format\021format-extra-args\030format-insufficient-a"
    "rgs\030format-invalid-specifier\016format-non-iso\021format-nonliteral\017"
    "format-overflow\033format-overflow-non-kprintf\017format-pedantic\017fo"
    "rmat-security\021format-signedness\021format-truncation\035format-trunc"
    "ation-non-kprintf\025format-type-confusion\nformat-y2k\022format-zero-l"
    "ength\010format=2\016fortify-source\023four-char-constants\015frame-add"
    "ress\021frame-larger-than\022frame-larger-than=%framework-include-priva"
    "te-from-public\023free-nonheap-object\013friend-enum\036function-def-in"
    "-objc-container\025function-multiversion\014fuse-ld-path\033future-attr"
    "ibute-extensions\015future-compat\ngcc-compat\026generic-type-extension"
    "\023global-constructors\013global-isel\003gnu\026gnu-alignof-expression"
    "\024gnu-anonymous-struct\033gnu-array-member-paren-init\015gnu-auto-typ"
    "e\022gnu-binary-literal\016gnu-case-range\023gnu-complex-integer gnu-co"
    "mpound-literal-initializer\037gnu-conditional-omitted-operand\016gnu-de"
    "signator\025gnu-empty-initializer\020gnu-empty-struct\036gnu-flexible-a"
    "rray-initializer\037gnu-flexible-array-union-member\024gnu-folding-cons"
    "tant\026gnu-imaginary-constant\020gnu-include-next\035gnu-inline-cpp-wi"
    "thout-extern\022gnu-label-as-value\017gnu-line-marker\033gnu-null-point"
    "er-arithmetic\027gnu-offsetof-extensions\021gnu-pointer-arith\023gnu-re"
    "declared-enum\030gnu-statement-expression-gnu-statement-expression-from"
    "-macro-expansion\025gnu-static-float-init$gnu-string-literal-operator-t"
    "emplate\016gnu-union-cast\"gnu-variable-sized-type-not-at-end\027gnu-ze"
    "ro-line-directive!gnu-zero-variadic-macro-arguments\024gpu-maybe-wrong-"
    "side\014header-guard\016header-hygiene\023higher-precision-fp\031hip-om"
    "p-target-directives\010hip-only\017hlsl-extensions\025idiomatic-parenth"
    "eses\022ignored-attributes)ignored-availability-without-sdk-settings\013"
    "ignored-gch\035ignored-optimization-argument\030ignored-pragma-intrinsi"
    "c\027ignored-pragma-optimize\017ignored-pragmas\022ignored-qualifiers\034"
    "ignored-reference-qualifiers\010implicit\032implicit-atomic-properties#"
    "implicit-const-int-float-conversion*implicit-conversion-floating-point-"
    "to-bool implicit-exception-spec-mismatch\024implicit-fallthrough!implic"
    "it-fallthrough-per-function\037implicit-fixed-point-conversion\031impli"
    "cit-float-conversion\035implicit-function-declaration\014implicit-int\027"
    "implicit-int-conversion\035implicit-int-float-conversion\024implicit-re"
    "tain-self\033implicitly-unsigned-literal\006import&import-preprocessor-"
    "directive-pedantic\021inaccessible-base include-angled-in-module-purvie"
    "w\032include-next-absolute-path\033include-next-outside-header\033incom"
    "patible-exception-spec#incompatible-function-pointer-types*incompatible"
    "-function-pointer-types-strict\"incompatible-library-redeclaration\036i"
    "ncompatible-ms-pragma-section\026incompatible-ms-struct\032incompatible"
    "-pointer-types.incompatible-pointer-types-discards-qualifiers\032incomp"
    "atible-property-type\024incompatible-sysroot'incomplete-framework-modul"
    "e-declaration\031incomplete-implementation\021incomplete-module\035inco"
    "mplete-setjmp-declaration\023incomplete-umbrella\026inconsistent-dllimp"
    "ort(inconsistent-missing-destructor-override\035inconsistent-missing-ov"
    "erride\016increment-bool\022infinite-recursion\tinit-self\025initialize"
    "r-overrides\023injected-class-name\006inline\ninline-asm#inline-namespa"
    "ce-reopened-noninline\021inline-new-delete\024installapi-violation\"ins"
    "tantiation-after-specialization\016int-conversion\017int-conversions\023"
    "int-in-bool-context\023int-to-pointer-cast\030int-to-void-pointer-cast\020"
    "integer-overflow\035invalid-command-line-argument\021invalid-constexpr\033"
    "invalid-feature-combination\020invalid-iboutlet&invalid-initializer-fro"
    "m-system-header\035invalid-ios-deployment-target\030invalid-no-builtin-"
    "names\020invalid-noreturn\020invalid-offsetof invalid-or-nonexistent-di"
    "rectory\036invalid-partial-specialization\013invalid-pch\020invalid-pp-"
    "token\027invalid-source-encoding\035invalid-static-assert-message\023in"
    "valid-token-paste\032invalid-unevaluated-string\014invalid-utf8\020jump"
    "-seh-finally\016keyword-compat\015keyword-macro\026knr-promoted-paramet"
    "er\030language-extension-token\023large-by-value-copy\006liblto\017link"
    "er-warnings\022literal-conversion\015literal-range\030local-type-templa"
    "te-args\027logical-not-parentheses\026logical-op-parentheses\tlong-long"
    "\015loop-analysis\017macro-redefined\004main\020main-return-type\027mal"
    "formed-warning-check\036many-braces-around-scalar-init*mathematical-not"
    "ation-identifier-extension\nmax-tokens\021max-unsigned-zero\026memset-t"
    "ransposed-args\022memsize-comparison\021method-signatures\tmicrosoft\022"
    "microsoft-abstract\022microsoft-anon-tag\016microsoft-cast\021microsoft"
    "-charize\027microsoft-comment-paste\024microsoft-const-init\023microsof"
    "t-cpp-macro\"microsoft-default-arg-redefinition\031microsoft-drectve-se"
    "ction\025microsoft-end-of-file microsoft-enum-forward-reference\024micr"
    "osoft-enum-value\030microsoft-exception-spec\020microsoft-exists#micros"
    "oft-explicit-constructor-call\035microsoft-extra-qualification\024micro"
    "soft-fixed-enum\030microsoft-flexible-array\016microsoft-goto\033micros"
    "oft-inaccessible-base\021microsoft-include\036microsoft-init-from-prede"
    "fined\033microsoft-mutable-reference\031microsoft-pure-definition\032mi"
    "crosoft-redeclare-static\020microsoft-sealed\027microsoft-static-assert"
    "(microsoft-string-literal-from-predefined\022microsoft-template\031micr"
    "osoft-template-shadow microsoft-union-member-reference\034microsoft-unq"
    "ualified-friend\024microsoft-using-decl\032microsoft-void-pseudo-dtor\t"
    "misexpect\026misleading-indentation\025mismatched-new-delete\032mismatc"
    "hed-parameter-types\027mismatched-return-types\017mismatched-tags\016mi"
    "ssing-braces\021missing-constinit\024missing-declarations%missing-desig"
    "nated-field-initializers\026missing-exception-spec\032missing-field-ini"
    "tializers\030missing-format-attribute\024missing-include-dirs\032missin"
    "g-method-return-type\020missing-multilib\020missing-noescape\020missing"
    "-noreturn\030missing-prototype-for-cc\022missing-prototypes\025missing-"
    "selector-name\017missing-sysroot\035missing-variable-declarations\025mi"
    "sspelled-assumption\016mix-packoffset\014module-build\017module-conflic"
    "t\033module-file-config-mismatch\025module-file-extension\015module-imp"
    "ort\031module-import-in-extern-c\032module-include-translation\013modul"
    "e-lock\"modules-ambiguous-internal-linkage\037modules-import-nested-red"
    "undant\004most\004move\014msvc-include\016msvc-not-found\tmulti-gpu\tmu"
    "ltichar\023multiple-move-vbase\025nan-infinity-disabled\tnarrowing\021n"
    "ested-anon-types\016nested-externs\020new-returns-null\013newline-eof\007"
    "noderef\015noexcept-type\031non-c-typedef-for-linkage\007non-gcc\033non"
    "-literal-null-conversion'non-modular-include-in-framework-module\035non"
    "-modular-include-in-module\017non-pod-varargs\032non-power-of-two-align"
    "ment\020non-virtual-dtor\007nonnull\025nonportable-cfstrings\030nonport"
    "able-include-path!nonportable-private-apinotes-path(nonportable-private"
    "-system-apinotes-path\037nonportable-system-include-path!nonportable-ve"
    "ctor-initialization\024nontrivial-memaccess\023nsconsumed-mismatch\022n"
    "sreturns-mismatch\017null-arithmetic\016null-character\017null-conversi"
    "on\020null-dereference\027null-pointer-arithmetic\030null-pointer-subtr"
    "action\013nullability\030nullability-completeness\"nullability-complete"
    "ness-on-arrays\024nullability-declspec\025nullability-extension#nullabi"
    "lity-inferred-on-nested-type\036nullable-to-nonnull-conversion+objc-aut"
    "osynthesis-property-ivar-name-match\035objc-bool-constant-conversion\013"
    "objc-boxing\027objc-circular-container\016objc-cocoa-api\034objc-design"
    "ated-initializers\036objc-dictionary-duplicate-keys\"objc-duplicate-cat"
    "egory-definition\023objc-flexible-array\037objc-forward-class-redefinit"
    "ion\024objc-interface-ivars\024objc-literal-compare\027objc-literal-con"
    "version\027objc-macro-redefinition\021objc-messaging-id\022objc-method-"
    "access\037objc-missing-property-synthesis\030objc-missing-super-calls\032"
    "objc-multiple-method-names\"objc-noncopy-retain-block-property\032objc-"
    "nonunified-exceptions#objc-property-assign-on-object-type\034objc-prope"
    "rty-implementation\037objc-property-implicit-mismatch*objc-property-mat"
    "ches-cocoa-ownership-rule\032objc-property-no-attribute\027objc-propert"
    "y-synthesis#objc-protocol-method-implementation objc-protocol-property-"
    "synthesis\030objc-protocol-qualifiers\"objc-readonly-with-setter-proper"
    "ty\026objc-redundant-api-use\032objc-redundant-literal-use\017objc-root"
    "-class\025objc-signed-char-bool/objc-signed-char-bool-implicit-float-co"
    "nversion-objc-signed-char-bool-implicit-int-conversion\023objc-string-c"
    "ompare\031objc-string-concatenation\034objc-unsafe-perform-selector\003"
    "odr\016old-style-cast\024old-style-definition\007openacc\037openacc-dep"
    "recated-clause-alias\"openacc-self-if-potential-conflict\027opencl-unsu"
    "pported-rgba\006openmp\024openmp-51-extensions\016openmp-clauses\021ope"
    "nmp-extensions\020openmp-loop-form\016openmp-mapping\015openmp-target\027"
    "openmp-target-exception\016option-ignored!ordered-compare-function-poin"
    "ters\027out-of-line-declaration\025out-of-scope-function\014over-aligne"
    "d\010overflow\022overlength-strings\037overloaded-shift-op-parentheses\022"
    "overloaded-virtual\015override-init\017override-module\032overriding-me"
    "thod-mismatch\021overriding-option\006packed\016packed-non-pod\006padde"
    "d\017padded-bitfield\013parentheses\024parentheses-equality\024partial-"
    "availability\004pass\015pass-analysis\013pass-failed\013pass-missed\015"
    "pch-date-time\014pch-vfs-diff\010pedantic\026pedantic-core-features\017"
    "pedantic-macros\020pessimizing-move\015pointer-arith\027pointer-bool-co"
    "nversion\017pointer-compare\027pointer-integer-compare\014pointer-sign\024"
    "pointer-to-enum-cast\023pointer-to-int-cast\025pointer-type-mismatch\031"
    "poison-system-directories\033potentially-direct-selector potentially-ev"
    "aluated-expression\026pragma-clang-attribute\032pragma-once-outside-hea"
    "der\013pragma-pack\036pragma-pack-suspicious-include#pragma-system-head"
    "er-outside-header\007pragmas\020pre-c++14-compat\031pre-c++14-compat-pe"
    "dantic\020pre-c++17-compat\031pre-c++17-compat-pedantic\020pre-c++20-co"
    "mpat\031pre-c++20-compat-pedantic\020pre-c++23-compat\031pre-c++23-comp"
    "at-pedantic\020pre-c++26-compat\031pre-c++26-compat-pedantic\020pre-c++"
    "2c-compat\031pre-c++2c-compat-pedantic\016pre-c11-compat\027pre-c11-com"
    "pat-pedantic\016pre-c23-compat\027pre-c23-compat-pedantic\016pre-c2x-co"
    "mpat\027pre-c2x-compat-pedantic\024pre-openmp-51-compat&predefined-iden"
    "tifier-outside-function\016private-extern\016private-header\016private-"
    "module\025profile-instr-missing\031profile-instr-out-of-date\030profile"
    "-instr-unprofiled\032property-access-dot-syntax\033property-attribute-m"
    "ismatch\010protocol%protocol-property-synthesis-ambiguity\005psabi\025p"
    "trauth-null-pointers\032qualified-void-return-type\"quoted-include-in-f"
    "ramework-header\023range-loop-analysis\031range-loop-bind-reference\024"
    "range-loop-construct\017read-only-types\032readonly-iboutlet-property\015"
    "receiver-expr\026receiver-forward-class\027redeclared-class-member\026r"
    "edundant-consteval-if\017redundant-decls\016redundant-move\020redundant"
    "-parens\010register\026reinterpret-base-class\025remark-backend-plugin\007"
    "reorder\014reorder-ctor\021reorder-init-list\030requires-super-attribut"
    "e\021reserved-id-macro\023reserved-identifier\031reserved-macro-identif"
    "ier\032reserved-module-identifier\035reserved-user-defined-literal\022r"
    "estrict-expansion\031retained-language-linkage\021return-local-addr\017"
    "return-mismatch\024return-stack-address\017return-std-move\013return-ty"
    "pe\025return-type-c-linkage\020rewrite-not-bool\023round-trip-cc1-args\004"
    "rtti\020sanitize-address\025sarif-format-unstable\021search-path-usage\007"
    "section\010selector\026selector-type-mismatch\013self-assign\021self-as"
    "sign-field\026self-assign-overloaded\tself-move\034semicolon-before-met"
    "hod-body\010sentinel\016sequence-point\026serialized-diagnostics\006sha"
    "dow\nshadow-all\014shadow-field\033shadow-field-in-constructor$shadow-f"
    "ield-in-constructor-modified\013shadow-ivar\027shadow-uncaptured-local\024"
    "shift-count-negative\024shift-count-overflow\024shift-negative-value\024"
    "shift-op-parentheses\016shift-overflow\023shift-sign-overflow\020shorte"
    "n-64-to-32\014sign-compare\017sign-conversion\nsign-promo\024signed-enu"
    "m-bitfield\025signed-unsigned-wchar'single-bit-bitfield-constant-conver"
    "sion\025sizeof-array-argument\022sizeof-array-decay\020sizeof-array-div"
    "\022sizeof-pointer-div\030sizeof-pointer-memaccess\020slash-u-filename\014"
    "slh-asm-goto\nsloc-usage\027sometimes-uninitialized\nsource-mgr\023sour"
    "ce-uses-openacc\022source-uses-openmp\013spir-compat\014spirv-compat\017"
    "stack-exhausted\017stack-protector\021static-float-init\020static-in-in"
    "line$static-inline-explicit-instantiation\026static-local-in-inline\020"
    "static-self-init\023stdlibcxx-not-found\017strict-aliasing\021strict-al"
    "iasing=0\021strict-aliasing=1\021strict-aliasing=2\017strict-overflow\021"
    "strict-overflow=0\021strict-overflow=1\021strict-overflow=2\021strict-o"
    "verflow=3\021strict-overflow=4\021strict-overflow=5\"strict-potentially"
    "-direct-selector\036strict-primary-template-shadow\021strict-prototypes"
    "\025strict-selector-match\016string-compare\024string-concatenation\021"
    "string-conversion\020string-plus-char\017string-plus-int\024strlcpy-str"
    "lcat-size\014strncat-size\033suggest-destructor-override\020suggest-ove"
    "rride\033super-class-method-mismatch\020suspicious-bzero\024suspicious-"
    "memaccess\024swift-name-attribute\006switch\013switch-bool\016switch-de"
    "fault\013switch-enum\016sync-alignment%sync-fetch-and-nand-semantics-ch"
    "anged\005synth\036target-clones-mixed-specifiers\034tautological-bitwis"
    "e-compare\024tautological-compare\035tautological-constant-compare&taut"
    "ological-constant-in-range-compare*tautological-constant-out-of-range-c"
    "ompare\035tautological-negation-compare\036tautological-objc-bool-compa"
    "re\034tautological-overlap-compare\034tautological-pointer-compare\037t"
    "autological-type-limit-compare\036tautological-undefined-compare'tautol"
    "ogical-unsigned-char-zero-compare'tautological-unsigned-enum-zero-compa"
    "re\"tautological-unsigned-zero-compare tautological-value-range-compare"
    "\017tcb-enforcement\034template-in-declaration-name\032tentative-defini"
    "tion-array$tentative-definition-incomplete-type\015thread-safety\026thr"
    "ead-safety-analysis\030thread-safety-attributes\022thread-safety-beta\026"
    "thread-safety-negative\025thread-safety-precise\027thread-safety-refere"
    "nce\036thread-safety-reference-return\025thread-safety-verbose\ttrigrap"
    "hs\013type-limits\013type-safety\024typedef-redefinition\020typename-mi"
    "ssing\031unable-to-open-stats-file\020unaligned-access!unaligned-qualif"
    "ier-implicit-cast\030unavailable-declarations\023undeclared-selector\005"
    "undef\014undef-prefix\027undefined-arm-streaming\020undefined-arm-za\021"
    "undefined-arm-zt0\031undefined-bool-conversion\027undefined-func-templa"
    "te\020undefined-inline\022undefined-internal\027undefined-internal-type"
    "\032undefined-reinterpret-cast\026undefined-var-template\035underaligne"
    "d-exception-object\026unevaluated-expression\026unguarded-availability\032"
    "unguarded-availability-new\007unicode\021unicode-homoglyph\022unicode-w"
    "hitespace\022unicode-zero-width\015uninitialized\035uninitialized-const"
    "-reference\020unknown-argument\022unknown-assumption\022unknown-attribu"
    "tes\024unknown-cuda-version\022unknown-directives\027unknown-escape-seq"
    "uence\017unknown-pragmas\022unknown-sanitizers\026unknown-warning-optio"
    "n\032unnamed-type-template-args\035unneeded-internal-declaration\030unn"
    "eeded-member-function\031unqualified-std-cast-call\020unreachable-code\033"
    "unreachable-code-aggressive\026unreachable-code-break\034unreachable-co"
    "de-fallthrough\036unreachable-code-generic-assoc\037unreachable-code-lo"
    "op-increment\027unreachable-code-return\023unsafe-buffer-usage unsafe-b"
    "uffer-usage-in-container\013unsequenced\017unsupported-abi\017unsupport"
    "ed-abs\036unsupported-availability-guard\016unsupported-cb#unsupported-"
    "dll-base-class-template\036unsupported-floating-point-opt\022unsupporte"
    "d-friend\021unsupported-gpopt\017unsupported-nan\026unsupported-target-"
    "opt\026unsupported-visibility\037unusable-partial-specialization\006unu"
    "sed\017unused-argument\030unused-but-set-parameter\027unused-but-set-va"
    "riable\034unused-command-line-argument\021unused-comparison\025unused-c"
    "onst-variable\032unused-exception-parameter\017unused-function\032unuse"
    "d-getter-return-value\014unused-label\025unused-lambda-capture\024unuse"
    "d-local-typedef\025unused-local-typedefs\015unused-macros\026unused-mem"
    "ber-function\020unused-parameter\024unused-private-field\024unused-prop"
    "erty-ivar\015unused-result\017unused-template\014unused-value\017unused"
    "-variable\026unused-volatile-lvalue\026used-but-marked-unused\025user-d"
    "efined-literals\025user-defined-warnings\007varargs\017variadic-macros\015"
    "vec-elem-size\021vector-conversion\022vector-conversions\014vexing-pars"
    "e\nvisibility\003vla\021vla-cxx-extension\015vla-extension\033vla-exten"
    "sion-static-assert\031void-pointer-to-enum-cast\030void-pointer-to-int-"
    "cast\024void-ptr-dereference\025volatile-register-var\023wasm-exception"
    "-spec\025weak-template-vtables\014weak-vtables\020writable-strings\015w"
    "rite-strings\017xor-used-as-pow\035zero-as-null-pointer-constant\021zer"
    "o-length-array"};

#endif // GET_DIAG_ARRAYS


#ifdef DIAG_ENTRY
DIAG_ENTRY(anonymous_60 /*  */, 0, 0, /* DiagSubGroup0 */ 1, R"()")
DIAG_ENTRY(PoundPragmaMessage /* #pragma-messages */, 1, /* DiagArray1 */ 1, 0, R"()")
DIAG_ENTRY(PoundWarning /* #warnings */, 18, /* DiagArray2 */ 3, 0, R"()")
DIAG_ENTRY(anonymous_289 /* CFString-literal */, 28, /* DiagArray3 */ 5, 0, R"()")
DIAG_ENTRY(anonymous_59 /* CL4 */, 45, 0, /* DiagSubGroup4 */ 3, R"()")
DIAG_ENTRY(IndependentClassAttribute /* IndependentClass-attribute */, 49, /* DiagArray5 */ 7, 0, R"()")
DIAG_ENTRY(NSobjectAttribute /* NSObject-attribute */, 76, /* DiagArray6 */ 10, 0, R"()")
DIAG_ENTRY(AArch64SMEAttributes /* aarch64-sme-attributes */, 95, /* DiagArray7 */ 12, 0, R"()")
DIAG_ENTRY(anonymous_0 /* abi */, 118, 0, 0, R"()")
DIAG_ENTRY(AbsoluteValue /* absolute-value */, 122, /* DiagArray9 */ 15, 0, R"()")
DIAG_ENTRY(AbstractFinalClass /* abstract-final-class */, 137, /* DiagArray10 */ 20, 0, R"()")
DIAG_ENTRY(anonymous_283 /* abstract-vbase-init */, 158, /* DiagArray11 */ 22, 0, R"()")
DIAG_ENTRY(anonymous_58 /* address */, 178, 0, /* DiagSubGroup12 */ 6, R"()")
DIAG_ENTRY(anonymous_259 /* address-of-packed-member */, 186, /* DiagArray13 */ 24, 0, R"()")
DIAG_ENTRY(AddressOfTemporary /* address-of-temporary */, 211, /* DiagArray14 */ 26, 0, R"()")
DIAG_ENTRY(anonymous_2 /* aggregate-return */, 232, 0, 0, R"()")
DIAG_ENTRY(AIXCompat /* aix-compat */, 249, /* DiagArray16 */ 28, 0, R"()")
DIAG_ENTRY(anonymous_271 /* alias-template-in-declaration-name */, 260, /* DiagArray17 */ 31, 0, R"()")
DIAG_ENTRY(anonymous_260 /* align-mismatch */, 295, /* DiagArray18 */ 33, 0, R"()")
DIAG_ENTRY(All /* all */, 310, 0, /* DiagSubGroup19 */ 10, R"()")
DIAG_ENTRY(anonymous_203 /* alloca */, 314, /* DiagArray20 */ 35, 0, R"()")
DIAG_ENTRY(anonymous_204 /* alloca-with-align-alignof */, 321, /* DiagArray21 */ 37, 0, R"()")
DIAG_ENTRY(AlwaysInlineCoroutine /* always-inline-coroutine */, 347, /* DiagArray22 */ 39, 0, R"()")
DIAG_ENTRY(anonymous_266 /* ambiguous-delete */, 371, /* DiagArray23 */ 41, 0, R"()")
DIAG_ENTRY(anonymous_154 /* ambiguous-ellipsis */, 388, /* DiagArray24 */ 43, 0, R"()")
DIAG_ENTRY(AmbiguousMacro /* ambiguous-macro */, 407, /* DiagArray25 */ 45, 0, R"()")
DIAG_ENTRY(AmbigMemberTemplate /* ambiguous-member-template */, 423, /* DiagArray26 */ 47, 0, R"()")
DIAG_ENTRY(anonymous_224 /* ambiguous-reversed-operator */, 449, /* DiagArray27 */ 49, 0, R"()")
DIAG_ENTRY(anonymous_109 /* analyzer-incompatible-plugin */, 477, /* DiagArray28 */ 51, 0, R"()")
DIAG_ENTRY(anonymous_106 /* android-unversioned-fallback */, 506, /* DiagArray29 */ 53, 0, R"()")
DIAG_ENTRY(AnonEnumEnumConversion /* anon-enum-enum-conversion */, 535, /* DiagArray30 */ 55, /* DiagSubGroup30 */ 18, R"()")
DIAG_ENTRY(anonymous_155 /* anonymous-pack-parens */, 561, /* DiagArray31 */ 57, 0, R"()")
DIAG_ENTRY(anonymous_91 /* apinotes */, 583, /* DiagArray32 */ 59, 0, R"()")
DIAG_ENTRY(AutomaticReferenceCounting /* arc */, 592, 0, /* DiagSubGroup33 */ 20, R"()")
DIAG_ENTRY(anonymous_153 /* arc-bridge-casts-disallowed-in-nonarc */, 596, /* DiagArray34 */ 61, 0, R"()")
DIAG_ENTRY(ARCRepeatedUseOfWeakMaybe /* arc-maybe-repeated-use-of-weak */, 634, /* DiagArray35 */ 63, 0, R"()")
DIAG_ENTRY(ARCNonPodMemAccess /* arc-non-pod-memaccess */, 665, /* DiagArray36 */ 65, 0, R"()")
DIAG_ENTRY(anonymous_183 /* arc-performSelector-leaks */, 687, /* DiagArray37 */ 67, 0, R"()")
DIAG_ENTRY(ARCRepeatedUseOfWeak /* arc-repeated-use-of-weak */, 713, /* DiagArray38 */ 69, /* DiagSubGroup38 */ 24, R"()")
DIAG_ENTRY(ARCRetainCycles /* arc-retain-cycles */, 738, /* DiagArray39 */ 71, 0, R"()")
DIAG_ENTRY(ARCUnsafeRetainedAssign /* arc-unsafe-retained-assign */, 756, /* DiagArray40 */ 73, 0, R"()")
DIAG_ENTRY(anonymous_291 /* argument-outside-range */, 783, /* DiagArray41 */ 77, 0, R"()")
DIAG_ENTRY(anonymous_292 /* argument-undefined-behaviour */, 806, /* DiagArray42 */ 79, 0, R"()")
DIAG_ENTRY(ArrayBounds /* array-bounds */, 835, /* DiagArray43 */ 81, 0, R"()")
DIAG_ENTRY(ArrayBoundsPointerArithmetic /* array-bounds-pointer-arithmetic */, 848, /* DiagArray44 */ 87, 0, R"()")
DIAG_ENTRY(ArrayParameter /* array-parameter */, 880, /* DiagArray45 */ 90, 0, R"()")
DIAG_ENTRY(ASM /* asm */, 896, 0, /* DiagSubGroup46 */ 26, R"()")
DIAG_ENTRY(ASMOperandWidths /* asm-operand-widths */, 900, /* DiagArray47 */ 92, 0, R"()")
DIAG_ENTRY(anonymous_290 /* assign-enum */, 919, /* DiagArray48 */ 94, 0, R"()")
DIAG_ENTRY(anonymous_170 /* assume */, 931, /* DiagArray49 */ 96, 0, R"()")
DIAG_ENTRY(anonymous_57 /* at-protocol */, 938, 0, 0, R"()")
DIAG_ENTRY(FrameworkHdrAtImport /* atimport-in-framework-header */, 950, /* DiagArray51 */ 98, 0, R"()")
DIAG_ENTRY(anonymous_247 /* atomic-access */, 979, /* DiagArray52 */ 100, 0, R"()")
DIAG_ENTRY(AtomicAlignment /* atomic-alignment */, 993, /* DiagArray53 */ 102, 0, R"()")
DIAG_ENTRY(anonymous_276 /* atomic-implicit-seq-cst */, 1010, /* DiagArray54 */ 105, 0, R"()")
DIAG_ENTRY(anonymous_275 /* atomic-memory-ordering */, 1034, /* DiagArray55 */ 107, 0, R"()")
DIAG_ENTRY(AtomicProperties /* atomic-properties */, 1057, 0, /* DiagSubGroup56 */ 28, R"()")
DIAG_ENTRY(anonymous_178 /* atomic-property-with-user-defined-accessor */, 1075, /* DiagArray57 */ 109, 0, R"()")
DIAG_ENTRY(anonymous_220 /* attribute-packed-for-bitfield */, 1118, /* DiagArray58 */ 111, 0, R"()")
DIAG_ENTRY(BackendWarningAttributes /* attribute-warning */, 1148, /* DiagArray59 */ 113, 0, R"()")
DIAG_ENTRY(Attributes /* attributes */, 1166, 0, /* DiagSubGroup60 */ 31, R"()")
DIAG_ENTRY(anonymous_161 /* auto-decl-extensions */, 1177, /* DiagArray61 */ 116, 0, R"()")
DIAG_ENTRY(AutoDisableVptrSanitizer /* auto-disable-vptr-sanitizer */, 1198, /* DiagArray62 */ 118, 0, R"()")
DIAG_ENTRY(anonymous_3 /* auto-import */, 1226, 0, 0, R"()")
DIAG_ENTRY(anonymous_151 /* auto-storage-class */, 1238, /* DiagArray64 */ 120, 0, R"()")
DIAG_ENTRY(anonymous_197 /* auto-var-id */, 1257, /* DiagArray65 */ 122, 0, R"()")
DIAG_ENTRY(Availability /* availability */, 1269, /* DiagArray66 */ 124, 0, R"()")
DIAG_ENTRY(AVRRtlibLinkingQuirks /* avr-rtlib-linking-quirks */, 1282, /* DiagArray67 */ 136, 0, R"()")
DIAG_ENTRY(BackendPlugin /* backend-plugin */, 1307, /* DiagArray68 */ 142, 0, R"()")
DIAG_ENTRY(anonymous_113 /* backslash-newline-escape */, 1322, /* DiagArray69 */ 145, 0, R"()")
DIAG_ENTRY(BadFunctionCast /* bad-function-cast */, 1347, /* DiagArray70 */ 147, 0, R"()")
DIAG_ENTRY(BinaryLiteral /* binary-literal */, 1365, 0, /* DiagSubGroup71 */ 34, R"()")
DIAG_ENTRY(BindToTemporaryCopy /* bind-to-temporary-copy */, 1380, /* DiagArray72 */ 149, /* DiagSubGroup72 */ 37, R"()")
DIAG_ENTRY(anonymous_165 /* binding-in-condition */, 1403, /* DiagArray73 */ 152, 0, R"()")
DIAG_ENTRY(BitIntExtension /* bit-int-extension */, 1424, /* DiagArray74 */ 154, 0, R"()")
DIAG_ENTRY(BitFieldConstantConversion /* bitfield-constant-conversion */, 1442, /* DiagArray75 */ 157, /* DiagSubGroup75 */ 39, R"()")
DIAG_ENTRY(BitFieldEnumConversion /* bitfield-enum-conversion */, 1471, /* DiagArray76 */ 159, 0, R"()")
DIAG_ENTRY(BitFieldWidth /* bitfield-width */, 1496, /* DiagArray77 */ 163, 0, R"()")
DIAG_ENTRY(BitwiseConditionalParentheses /* bitwise-conditional-parentheses */, 1511, /* DiagArray78 */ 165, 0, R"()")
DIAG_ENTRY(BitwiseInsteadOfLogical /* bitwise-instead-of-logical */, 1543, /* DiagArray79 */ 167, 0, R"()")
DIAG_ENTRY(BitwiseOpParentheses /* bitwise-op-parentheses */, 1570, /* DiagArray80 */ 169, 0, R"()")
DIAG_ENTRY(BlockCaptureAutoReleasing /* block-capture-autoreleasing */, 1593, /* DiagArray81 */ 171, 0, R"()")
DIAG_ENTRY(BoolConversion /* bool-conversion */, 1621, /* DiagArray82 */ 173, /* DiagSubGroup82 */ 41, R"()")
DIAG_ENTRY(anonymous_65 /* bool-conversions */, 1637, 0, /* DiagSubGroup83 */ 44, R"()")
DIAG_ENTRY(BoolOperation /* bool-operation */, 1654, /* DiagArray84 */ 175, /* DiagSubGroup84 */ 46, R"()")
DIAG_ENTRY(BoundsSafetyCountedByEltTyUnknownSize /* bounds-safety-counted-by-elt-type-unknown-size */, 1669, /* DiagArray85 */ 177, 0, R"()")
DIAG_ENTRY(anonymous_242 /* braced-scalar-init */, 1716, /* DiagArray86 */ 179, 0, R"()")
DIAG_ENTRY(BranchProtection /* branch-protection */, 1735, /* DiagArray87 */ 181, 0, R"()")
DIAG_ENTRY(ObjCBridge /* bridge-cast */, 1753, /* DiagArray88 */ 186, 0, R"()")
DIAG_ENTRY(anonymous_206 /* builtin-assume-aligned-alignment */, 1765, /* DiagArray89 */ 189, 0, R"()")
DIAG_ENTRY(BuiltinMacroRedefined /* builtin-macro-redefined */, 1798, /* DiagArray90 */ 191, 0, R"()")
DIAG_ENTRY(anonymous_171 /* builtin-memcpy-chk-size */, 1822, /* DiagArray91 */ 194, 0, R"()")
DIAG_ENTRY(BuiltinRequiresHeader /* builtin-requires-header */, 1846, /* DiagArray92 */ 196, 0, R"()")
DIAG_ENTRY(CXXCompat /* c++-compat */, 1870, /* DiagArray93 */ 198, 0, R"()")
DIAG_ENTRY(anonymous_25 /* c++0x-compat */, 1881, 0, /* DiagSubGroup94 */ 48, R"()")
DIAG_ENTRY(anonymous_69 /* c++0x-extensions */, 1894, 0, /* DiagSubGroup95 */ 50, R"()")
DIAG_ENTRY(anonymous_23 /* c++0x-narrowing */, 1911, 0, /* DiagSubGroup96 */ 52, R"()")
DIAG_ENTRY(CXX11Compat /* c++11-compat */, 1927, /* DiagArray97 */ 200, /* DiagSubGroup97 */ 54, R"()")
DIAG_ENTRY(CXX11CompatDeprecatedWritableStr /* c++11-compat-deprecated-writable-strings */, 1940, /* DiagArray98 */ 211, 0, R"()")
DIAG_ENTRY(CXX11CompatPedantic /* c++11-compat-pedantic */, 1981, 0, /* DiagSubGroup99 */ 62, R"()")
DIAG_ENTRY(CXX11CompatReservedUserDefinedLiteral /* c++11-compat-reserved-user-defined-literal */, 2003, /* DiagArray100 */ 213, 0, R"()")
DIAG_ENTRY(CXX11 /* c++11-extensions */, 2046, /* DiagArray101 */ 215, /* DiagSubGroup101 */ 68, R"()")
DIAG_ENTRY(CXX11ExtraSemi /* c++11-extra-semi */, 2063, /* DiagArray102 */ 242, 0, R"()")
DIAG_ENTRY(CXX11InlineNamespace /* c++11-inline-namespace */, 2080, /* DiagArray103 */ 244, 0, R"()")
DIAG_ENTRY(CXX11LongLong /* c++11-long-long */, 2103, /* DiagArray104 */ 246, 0, R"()")
DIAG_ENTRY(CXX11Narrowing /* c++11-narrowing */, 2119, /* DiagArray105 */ 248, /* DiagSubGroup105 */ 72, R"()")
DIAG_ENTRY(CXX11NarrowingConstReference /* c++11-narrowing-const-reference */, 2135, /* DiagArray106 */ 256, 0, R"()")
DIAG_ENTRY(CXX14Attrs /* c++14-attribute-extensions */, 2167, /* DiagArray107 */ 260, 0, R"()")
DIAG_ENTRY(CXX14BinaryLiteral /* c++14-binary-literal */, 2194, /* DiagArray108 */ 262, 0, R"()")
DIAG_ENTRY(CXX14Compat /* c++14-compat */, 2215, 0, /* DiagSubGroup109 */ 74, R"()")
DIAG_ENTRY(CXX14CompatPedantic /* c++14-compat-pedantic */, 2228, 0, /* DiagSubGroup110 */ 78, R"()")
DIAG_ENTRY(CXX14 /* c++14-extensions */, 2250, /* DiagArray111 */ 264, /* DiagSubGroup111 */ 83, R"()")
DIAG_ENTRY(CXX17Attrs /* c++17-attribute-extensions */, 2267, /* DiagArray112 */ 272, 0, R"()")
DIAG_ENTRY(CXX17Compat /* c++17-compat */, 2294, 0, /* DiagSubGroup113 */ 86, R"()")
DIAG_ENTRY(CXX17CompatMangling /* c++17-compat-mangling */, 2307, /* DiagArray114 */ 274, 0, R"()")
DIAG_ENTRY(CXX17CompatPedantic /* c++17-compat-pedantic */, 2329, 0, /* DiagSubGroup115 */ 92, R"()")
DIAG_ENTRY(CXX17 /* c++17-extensions */, 2351, /* DiagArray116 */ 276, /* DiagSubGroup116 */ 96, R"()")
DIAG_ENTRY(anonymous_70 /* c++1y-extensions */, 2368, 0, /* DiagSubGroup117 */ 98, R"()")
DIAG_ENTRY(anonymous_26 /* c++1z-compat */, 2385, 0, /* DiagSubGroup118 */ 100, R"()")
DIAG_ENTRY(anonymous_12 /* c++1z-compat-mangling */, 2398, 0, /* DiagSubGroup119 */ 102, R"()")
DIAG_ENTRY(anonymous_71 /* c++1z-extensions */, 2420, 0, /* DiagSubGroup120 */ 104, R"()")
DIAG_ENTRY(CXX20Attrs /* c++20-attribute-extensions */, 2437, /* DiagArray121 */ 294, 0, R"()")
DIAG_ENTRY(CXX20Compat /* c++20-compat */, 2464, /* DiagArray122 */ 296, /* DiagSubGroup122 */ 106, R"()")
DIAG_ENTRY(CXX20CompatPedantic /* c++20-compat-pedantic */, 2477, 0, /* DiagSubGroup123 */ 108, R"()")
DIAG_ENTRY(CXX20Designator /* c++20-designator */, 2499, /* DiagArray124 */ 306, 0, R"()")
DIAG_ENTRY(CXX20 /* c++20-extensions */, 2516, /* DiagArray125 */ 308, /* DiagSubGroup125 */ 111, R"()")
DIAG_ENTRY(CXX23Attrs /* c++23-attribute-extensions */, 2533, /* DiagArray126 */ 331, 0, R"()")
DIAG_ENTRY(CXX23 /* c++23-extensions */, 2560, /* DiagArray127 */ 333, /* DiagSubGroup127 */ 114, R"()")
DIAG_ENTRY(CXX23AttrsOnLambda /* c++23-lambda-attributes */, 2577, /* DiagArray128 */ 344, 0, R"()")
DIAG_ENTRY(CXX26 /* c++26-extensions */, 2601, /* DiagArray129 */ 346, 0, R"()")
DIAG_ENTRY(anonymous_27 /* c++2a-compat */, 2618, 0, /* DiagSubGroup130 */ 116, R"()")
DIAG_ENTRY(anonymous_28 /* c++2a-compat-pedantic */, 2631, 0, /* DiagSubGroup131 */ 118, R"()")
DIAG_ENTRY(anonymous_72 /* c++2a-extensions */, 2653, 0, /* DiagSubGroup132 */ 120, R"()")
DIAG_ENTRY(anonymous_73 /* c++2b-extensions */, 2670, 0, /* DiagSubGroup133 */ 122, R"()")
DIAG_ENTRY(anonymous_74 /* c++2c-extensions */, 2687, 0, /* DiagSubGroup134 */ 124, R"()")
DIAG_ENTRY(anonymous_19 /* c++98-c++11-c++14-c++17-compat */, 2704, 0, /* DiagSubGroup135 */ 126, R"()")
DIAG_ENTRY(anonymous_20 /* c++98-c++11-c++14-c++17-compat-pedantic */, 2735, 0, /* DiagSubGroup136 */ 128, R"()")
DIAG_ENTRY(anonymous_17 /* c++98-c++11-c++14-compat */, 2775, 0, /* DiagSubGroup137 */ 130, R"()")
DIAG_ENTRY(anonymous_18 /* c++98-c++11-c++14-compat-pedantic */, 2800, 0, /* DiagSubGroup138 */ 132, R"()")
DIAG_ENTRY(anonymous_15 /* c++98-c++11-compat */, 2834, 0, /* DiagSubGroup139 */ 134, R"()")
DIAG_ENTRY(CXXPre14CompatBinaryLiteral /* c++98-c++11-compat-binary-literal */, 2853, /* DiagArray140 */ 351, 0, R"()")
DIAG_ENTRY(anonymous_16 /* c++98-c++11-compat-pedantic */, 2887, 0, /* DiagSubGroup141 */ 136, R"()")
DIAG_ENTRY(CXX98Compat /* c++98-compat */, 2915, /* DiagArray142 */ 353, /* DiagSubGroup142 */ 138, R"()")
DIAG_ENTRY(CXX98CompatBindToTemporaryCopy /* c++98-compat-bind-to-temporary-copy */, 2928, /* DiagArray143 */ 412, 0, R"()")
DIAG_ENTRY(CXX98CompatExtraSemi /* c++98-compat-extra-semi */, 2964, /* DiagArray144 */ 414, 0, R"()")
DIAG_ENTRY(CXX98CompatLocalTypeTemplateArgs /* c++98-compat-local-type-template-args */, 2988, /* DiagArray145 */ 416, 0, R"()")
DIAG_ENTRY(CXX98CompatPedantic /* c++98-compat-pedantic */, 3026, /* DiagArray146 */ 418, /* DiagSubGroup146 */ 145, R"()")
DIAG_ENTRY(CXX98CompatUnnamedTypeTemplateArgs /* c++98-compat-unnamed-type-template-args */, 3048, /* DiagArray147 */ 428, 0, R"()")
DIAG_ENTRY(C11 /* c11-extensions */, 3088, /* DiagArray148 */ 430, 0, R"()")
DIAG_ENTRY(C23Compat /* c23-compat */, 3103, /* DiagArray149 */ 435, 0, R"()")
DIAG_ENTRY(C23 /* c23-extensions */, 3114, /* DiagArray150 */ 437, 0, R"()")
DIAG_ENTRY(anonymous_4 /* c2x-compat */, 3129, 0, /* DiagSubGroup151 */ 153, R"()")
DIAG_ENTRY(anonymous_75 /* c2x-extensions */, 3140, 0, /* DiagSubGroup152 */ 155, R"()")
DIAG_ENTRY(C99Compat /* c99-compat */, 3155, /* DiagArray153 */ 450, 0, R"()")
DIAG_ENTRY(C99Designator /* c99-designator */, 3166, /* DiagArray154 */ 455, /* DiagSubGroup154 */ 157, R"()")
DIAG_ENTRY(C99 /* c99-extensions */, 3181, /* DiagArray155 */ 461, /* DiagSubGroup155 */ 159, R"()")
DIAG_ENTRY(PureVirtualCallFromCtorDtor /* call-to-pure-virtual-from-ctor-dtor */, 3196, /* DiagArray156 */ 472, 0, R"()")
DIAG_ENTRY(CalledOnceParameter /* called-once-parameter */, 3232, /* DiagArray157 */ 474, /* DiagSubGroup157 */ 161, R"()")
DIAG_ENTRY(CastAlign /* cast-align */, 3254, /* DiagArray158 */ 478, 0, R"()")
DIAG_ENTRY(anonymous_278 /* cast-calling-convention */, 3265, /* DiagArray159 */ 480, 0, R"()")
DIAG_ENTRY(CastFunctionType /* cast-function-type */, 3289, 0, /* DiagSubGroup160 */ 163, R"()")
DIAG_ENTRY(CastFunctionTypeMismatch /* cast-function-type-mismatch */, 3308, /* DiagArray161 */ 482, 0, R"()")
DIAG_ENTRY(CastFunctionTypeStrict /* cast-function-type-strict */, 3336, /* DiagArray162 */ 484, 0, R"()")
DIAG_ENTRY(SelTypeCast /* cast-of-sel-type */, 3362, /* DiagArray163 */ 486, 0, R"()")
DIAG_ENTRY(CastQual /* cast-qual */, 3379, /* DiagArray164 */ 488, 0, R"()")
DIAG_ENTRY(anonymous_264 /* cast-qual-unrelated */, 3389, /* DiagArray165 */ 491, 0, R"()")
DIAG_ENTRY(anonymous_5 /* char-align */, 3409, 0, 0, R"()")
DIAG_ENTRY(CharSubscript /* char-subscripts */, 3420, /* DiagArray167 */ 493, 0, R"()")
DIAG_ENTRY(ClangClPch /* clang-cl-pch */, 3436, /* DiagArray168 */ 496, 0, R"()")
DIAG_ENTRY(ClassConversion /* class-conversion */, 3449, /* DiagArray169 */ 501, 0, R"()")
DIAG_ENTRY(ClassVarargs /* class-varargs */, 3466, /* DiagArray170 */ 505, /* DiagSubGroup170 */ 166, R"()")
DIAG_ENTRY(anonymous_207 /* cmse-union-leak */, 3480, /* DiagArray171 */ 507, 0, R"()")
DIAG_ENTRY(anonymous_159 /* comma */, 3496, /* DiagArray172 */ 509, 0, R"()")
DIAG_ENTRY(Comment /* comment */, 3502, /* DiagArray173 */ 511, 0, R"()")
DIAG_ENTRY(anonymous_63 /* comments */, 3510, 0, /* DiagSubGroup174 */ 168, R"()")
DIAG_ENTRY(CompareDistinctPointerType /* compare-distinct-pointer-types */, 3519, /* DiagArray175 */ 516, 0, R"()")
DIAG_ENTRY(CompletionHandler /* completion-handler */, 3550, /* DiagArray176 */ 518, 0, R"()")
DIAG_ENTRY(anonymous_244 /* complex-component-init */, 3569, /* DiagArray177 */ 522, 0, R"()")
DIAG_ENTRY(CompoundTokenSplit /* compound-token-split */, 3592, 0, /* DiagSubGroup178 */ 170, R"()")
DIAG_ENTRY(CompoundTokenSplitByMacro /* compound-token-split-by-macro */, 3613, /* DiagArray179 */ 524, 0, R"()")
DIAG_ENTRY(CompoundTokenSplitBySpace /* compound-token-split-by-space */, 3643, /* DiagArray180 */ 526, 0, R"()")
DIAG_ENTRY(anonymous_280 /* conditional-type-mismatch */, 3673, /* DiagArray181 */ 528, 0, R"()")
DIAG_ENTRY(UninitializedMaybe /* conditional-uninitialized */, 3699, /* DiagArray182 */ 530, 0, R"()")
DIAG_ENTRY(ConfigMacros /* config-macros */, 3725, /* DiagArray183 */ 532, 0, R"()")
DIAG_ENTRY(ConstantConversion /* constant-conversion */, 3739, /* DiagArray184 */ 534, /* DiagSubGroup184 */ 173, R"()")
DIAG_ENTRY(anonymous_82 /* constant-evaluated */, 3759, /* DiagArray185 */ 536, 0, R"()")
DIAG_ENTRY(anonymous_254 /* constant-logical-operand */, 3778, /* DiagArray186 */ 538, 0, R"()")
DIAG_ENTRY(anonymous_195 /* constexpr-not-const */, 3803, /* DiagArray187 */ 540, 0, R"()")
DIAG_ENTRY(Consumed /* consumed */, 3823, /* DiagArray188 */ 542, 0, R"()")
DIAG_ENTRY(Conversion /* conversion */, 3832, /* DiagArray189 */ 551, /* DiagSubGroup189 */ 176, R"()")
DIAG_ENTRY(anonymous_64 /* conversion-null */, 3843, 0, /* DiagSubGroup190 */ 191, R"()")
DIAG_ENTRY(CoroNonAlignedAllocationFunction /* coro-non-aligned-allocation-function */, 3859, /* DiagArray191 */ 558, 0, R"()")
DIAG_ENTRY(Coroutine /* coroutine */, 3896, /* DiagArray192 */ 560, /* DiagSubGroup192 */ 193, R"()")
DIAG_ENTRY(CoroutineMissingUnhandledException /* coroutine-missing-unhandled-exception */, 3906, /* DiagArray193 */ 562, 0, R"()")
DIAG_ENTRY(CoveredSwitchDefault /* covered-switch-default */, 3944, /* DiagArray194 */ 564, 0, R"()")
DIAG_ENTRY(anonymous_62 /* cpp */, 3967, 0, /* DiagSubGroup195 */ 198, R"()")
DIAG_ENTRY(ObjCCStringFormat /* cstring-format-directive */, 3971, /* DiagArray196 */ 566, 0, R"()")
DIAG_ENTRY(CTADMaybeUnsupported /* ctad-maybe-unsupported */, 3996, /* DiagArray197 */ 568, 0, R"()")
DIAG_ENTRY(anonymous_6 /* ctor-dtor-privacy */, 4019, 0, 0, R"()")
DIAG_ENTRY(CrossTU /* ctu */, 4037, /* DiagArray199 */ 570, 0, R"()")
DIAG_ENTRY(CudaCompat /* cuda-compat */, 4041, /* DiagArray200 */ 572, 0, R"()")
DIAG_ENTRY(CustomAtomic /* custom-atomic-properties */, 4053, /* DiagArray201 */ 578, 0, R"()")
DIAG_ENTRY(anonymous_156 /* cxx-attribute-extension */, 4078, /* DiagArray202 */ 580, 0, R"()")
DIAG_ENTRY(Dangling /* dangling */, 4102, /* DiagArray203 */ 582, /* DiagSubGroup203 */ 200, R"()")
DIAG_ENTRY(DanglingElse /* dangling-else */, 4111, /* DiagArray204 */ 584, 0, R"()")
DIAG_ENTRY(DanglingField /* dangling-field */, 4125, /* DiagArray205 */ 586, 0, R"()")
DIAG_ENTRY(DanglingGsl /* dangling-gsl */, 4140, /* DiagArray206 */ 591, 0, R"()")
DIAG_ENTRY(DanglingInitializerList /* dangling-initializer-list */, 4153, /* DiagArray207 */ 594, 0, R"()")
DIAG_ENTRY(anonymous_102 /* darwin-sdk-settings */, 4179, /* DiagArray208 */ 596, 0, R"()")
DIAG_ENTRY(anonymous_139 /* date-time */, 4199, /* DiagArray209 */ 598, 0, R"()")
DIAG_ENTRY(DeallocInCategory /* dealloc-in-category */, 4209, /* DiagArray210 */ 600, 0, R"()")
DIAG_ENTRY(anonymous_98 /* debug-compression-unavailable */, 4229, /* DiagArray211 */ 602, 0, R"()")
DIAG_ENTRY(DeclarationAfterStatement /* declaration-after-statement */, 4259, /* DiagArray212 */ 604, 0, R"()")
DIAG_ENTRY(DefaultedFunctionDeleted /* defaulted-function-deleted */, 4287, /* DiagArray213 */ 607, 0, R"()")
DIAG_ENTRY(anonymous_99 /* delayed-template-parsing-in-cxx20 */, 4314, /* DiagArray214 */ 610, 0, R"()")
DIAG_ENTRY(DelegatingCtorCycles /* delegating-ctor-cycles */, 4348, /* DiagArray215 */ 612, 0, R"()")
DIAG_ENTRY(DeleteAbstractNonVirtualDtor /* delete-abstract-non-virtual-dtor */, 4371, /* DiagArray216 */ 614, 0, R"()")
DIAG_ENTRY(DeleteIncomplete /* delete-incomplete */, 4404, /* DiagArray217 */ 616, 0, R"()")
DIAG_ENTRY(DeleteNonAbstractNonVirtualDtor /* delete-non-abstract-non-virtual-dtor */, 4422, /* DiagArray218 */ 619, 0, R"()")
DIAG_ENTRY(DeleteNonVirtualDtor /* delete-non-virtual-dtor */, 4459, 0, /* DiagSubGroup219 */ 205, R"()")
DIAG_ENTRY(anonymous_121 /* delimited-escape-sequence-extension */, 4483, /* DiagArray220 */ 621, 0, R"()")
DIAG_ENTRY(anonymous_268 /* deprecate-lax-vec-conv-all */, 4519, /* DiagArray221 */ 623, 0, R"()")
DIAG_ENTRY(Deprecated /* deprecated */, 4546, /* DiagArray222 */ 625, /* DiagSubGroup222 */ 208, R"()")
DIAG_ENTRY(anonymous_267 /* deprecated-altivec-src-compat */, 4557, /* DiagArray223 */ 635, 0, R"()")
DIAG_ENTRY(DeprecatedAnonEnumEnumConversion /* deprecated-anon-enum-enum-conversion */, 4587, /* DiagArray224 */ 637, 0, R"()")
DIAG_ENTRY(DeprecatedArrayCompare /* deprecated-array-compare */, 4624, /* DiagArray225 */ 639, 0, R"()")
DIAG_ENTRY(DeprecatedAttributes /* deprecated-attributes */, 4649, /* DiagArray226 */ 641, 0, R"()")
DIAG_ENTRY(DeprecatedBuiltins /* deprecated-builtins */, 4671, /* DiagArray227 */ 645, 0, R"()")
DIAG_ENTRY(DeprecatedCommaSubscript /* deprecated-comma-subscript */, 4691, /* DiagArray228 */ 647, 0, R"()")
DIAG_ENTRY(DeprecatedCopy /* deprecated-copy */, 4718, /* DiagArray229 */ 649, /* DiagSubGroup229 */ 232, R"()")
DIAG_ENTRY(anonymous_7 /* deprecated-copy-dtor */, 4734, 0, /* DiagSubGroup230 */ 234, R"()")
DIAG_ENTRY(DeprecatedCopyWithDtor /* deprecated-copy-with-dtor */, 4755, /* DiagArray231 */ 651, /* DiagSubGroup231 */ 236, R"()")
DIAG_ENTRY(DeprecatedCopyWithUserProvidedCopy /* deprecated-copy-with-user-provided-copy */, 4781, /* DiagArray232 */ 653, 0, R"()")
DIAG_ENTRY(DeprecatedCopyWithUserProvidedDtor /* deprecated-copy-with-user-provided-dtor */, 4821, /* DiagArray233 */ 655, 0, R"()")
DIAG_ENTRY(DeprecatedCoroutine /* deprecated-coroutine */, 4861, /* DiagArray234 */ 657, 0, R"()")
DIAG_ENTRY(DeprecatedDeclarations /* deprecated-declarations */, 4882, /* DiagArray235 */ 659, 0, R"()")
DIAG_ENTRY(DeprecatedDynamicExceptionSpec /* deprecated-dynamic-exception-spec */, 4906, /* DiagArray236 */ 666, 0, R"()")
DIAG_ENTRY(DeprecatedEnumCompare /* deprecated-enum-compare */, 4940, /* DiagArray237 */ 668, 0, R"()")
DIAG_ENTRY(DeprecatedEnumCompareConditional /* deprecated-enum-compare-conditional */, 4964, /* DiagArray238 */ 670, 0, R"()")
DIAG_ENTRY(DeprecatedEnumEnumConversion /* deprecated-enum-enum-conversion */, 5000, /* DiagArray239 */ 672, 0, R"()")
DIAG_ENTRY(DeprecatedEnumFloatConversion /* deprecated-enum-float-conversion */, 5032, /* DiagArray240 */ 674, 0, R"()")
DIAG_ENTRY(DeprecatedImplementations /* deprecated-implementations */, 5065, /* DiagArray241 */ 676, 0, R"()")
DIAG_ENTRY(DeprecatedIncrementBool /* deprecated-increment-bool */, 5092, /* DiagArray242 */ 679, 0, R"()")
DIAG_ENTRY(DeprecatedLiteralOperator /* deprecated-literal-operator */, 5118, /* DiagArray243 */ 681, 0, R"()")
DIAG_ENTRY(DeprecatedModuleDotMap /* deprecated-module-dot-map */, 5146, /* DiagArray244 */ 683, 0, R"()")
DIAG_ENTRY(DeprecatedNoRelaxedTemplateTemplateArgs /* deprecated-no-relaxed-template-template-args */, 5172, /* DiagArray245 */ 685, 0, R"()")
DIAG_ENTRY(DeprecatedNonPrototype /* deprecated-non-prototype */, 5217, /* DiagArray246 */ 687, 0, R"()")
DIAG_ENTRY(DeprecatedObjCIsaUsage /* deprecated-objc-isa-usage */, 5242, /* DiagArray247 */ 690, 0, R"()")
DIAG_ENTRY(ObjCPointerIntrospect /* deprecated-objc-pointer-introspection */, 5268, /* DiagArray248 */ 693, /* DiagSubGroup248 */ 238, R"()")
DIAG_ENTRY(ObjCPointerIntrospectPerformSelector /* deprecated-objc-pointer-introspection-performSelector */, 5306, /* DiagArray249 */ 695, 0, R"()")
DIAG_ENTRY(DeprecatedPragma /* deprecated-pragma */, 5360, /* DiagArray250 */ 697, 0, R"()")
DIAG_ENTRY(DeprecatedRedundantConstexprStaticDef /* deprecated-redundant-constexpr-static-def */, 5378, /* DiagArray251 */ 699, 0, R"()")
DIAG_ENTRY(DeprecatedRegister /* deprecated-register */, 5420, /* DiagArray252 */ 701, 0, R"()")
DIAG_ENTRY(DeprecatedThisCapture /* deprecated-this-capture */, 5440, /* DiagArray253 */ 703, 0, R"()")
DIAG_ENTRY(DeprecatedType /* deprecated-type */, 5464, /* DiagArray254 */ 705, 0, R"()")
DIAG_ENTRY(DeprecatedVolatile /* deprecated-volatile */, 5480, /* DiagArray255 */ 707, 0, R"()")
DIAG_ENTRY(DeprecatedWritableStr /* deprecated-writable-strings */, 5500, 0, /* DiagSubGroup256 */ 240, R"()")
DIAG_ENTRY(anonymous_296 /* direct-ivar-access */, 5528, /* DiagArray257 */ 713, 0, R"()")
DIAG_ENTRY(anonymous_130 /* disabled-macro-expansion */, 5547, /* DiagArray258 */ 715, 0, R"()")
DIAG_ENTRY(anonymous_9 /* disabled-optimization */, 5572, 0, 0, R"()")
DIAG_ENTRY(anonymous_10 /* discard-qual */, 5594, 0, 0, R"()")
DIAG_ENTRY(DistributedObjectModifiers /* distributed-object-modifiers */, 5607, /* DiagArray261 */ 717, 0, R"()")
DIAG_ENTRY(anonymous_11 /* div-by-zero */, 5636, 0, /* DiagSubGroup262 */ 242, R"()")
DIAG_ENTRY(DivZero /* division-by-zero */, 5648, /* DiagArray263 */ 720, 0, R"()")
DIAG_ENTRY(anonymous_208 /* dll-attribute-on-redeclaration */, 5665, /* DiagArray264 */ 722, 0, R"()")
DIAG_ENTRY(DllexportExplicitInstantiationDecl /* dllexport-explicit-instantiation-decl */, 5696, /* DiagArray265 */ 724, 0, R"()")
DIAG_ENTRY(anonymous_209 /* dllimport-static-field-def */, 5734, /* DiagArray266 */ 726, 0, R"()")
DIAG_ENTRY(Documentation /* documentation */, 5761, /* DiagArray267 */ 728, /* DiagSubGroup267 */ 244, R"()")
DIAG_ENTRY(DocumentationDeprecatedSync /* documentation-deprecated-sync */, 5775, /* DiagArray268 */ 749, 0, R"()")
DIAG_ENTRY(DocumentationHTML /* documentation-html */, 5805, /* DiagArray269 */ 751, 0, R"()")
DIAG_ENTRY(DocumentationPedantic /* documentation-pedantic */, 5824, /* DiagArray270 */ 756, /* DiagSubGroup270 */ 247, R"()")
DIAG_ENTRY(DocumentationUnknownCommand /* documentation-unknown-command */, 5847, /* DiagArray271 */ 758, 0, R"()")
DIAG_ENTRY(anonymous_114 /* dollar-in-identifier-extension */, 5877, /* DiagArray272 */ 761, 0, R"()")
DIAG_ENTRY(DoublePromotion /* double-promotion */, 5908, /* DiagArray273 */ 763, 0, R"()")
DIAG_ENTRY(DtorName /* dtor-name */, 5925, /* DiagArray274 */ 765, 0, R"()")
DIAG_ENTRY(anonymous_194 /* dtor-typedef */, 5935, /* DiagArray275 */ 769, 0, R"()")
DIAG_ENTRY(DuplicateDeclSpecifier /* duplicate-decl-specifier */, 5948, /* DiagArray276 */ 771, 0, R"()")
DIAG_ENTRY(anonymous_158 /* duplicate-enum */, 5973, /* DiagArray277 */ 776, 0, R"()")
DIAG_ENTRY(DuplicateArgDecl /* duplicate-method-arg */, 5988, /* DiagArray278 */ 778, 0, R"()")
DIAG_ENTRY(MethodDuplicate /* duplicate-method-match */, 6009, /* DiagArray279 */ 780, 0, R"()")
DIAG_ENTRY(anonymous_173 /* duplicate-protocol */, 6032, /* DiagArray280 */ 782, 0, R"()")
DIAG_ENTRY(DXILValidation /* dxil-validation */, 6051, /* DiagArray281 */ 784, 0, R"()")
DIAG_ENTRY(DynamicClassMemaccess /* dynamic-class-memaccess */, 6067, /* DiagArray282 */ 786, 0, R"()")
DIAG_ENTRY(DynamicExceptionSpec /* dynamic-exception-spec */, 6091, /* DiagArray283 */ 788, /* DiagSubGroup283 */ 249, R"()")
DIAG_ENTRY(anonymous_111 /* eager-load-cxx-named-modules */, 6114, /* DiagArray284 */ 790, 0, R"()")
DIAG_ENTRY(anonymous_39 /* effc++ */, 6143, 0, /* DiagSubGroup285 */ 251, R"()")
DIAG_ENTRY(anonymous_146 /* elaborated-enum-base */, 6150, /* DiagArray286 */ 792, 0, R"()")
DIAG_ENTRY(anonymous_147 /* elaborated-enum-class */, 6171, /* DiagArray287 */ 794, 0, R"()")
DIAG_ENTRY(anonymous_136 /* embedded-directive */, 6193, /* DiagArray288 */ 796, 0, R"()")
DIAG_ENTRY(EmptyBody /* empty-body */, 6212, /* DiagArray289 */ 798, 0, R"()")
DIAG_ENTRY(anonymous_152 /* empty-decomposition */, 6223, /* DiagArray290 */ 804, 0, R"()")
DIAG_ENTRY(EmptyInitStatement /* empty-init-stmt */, 6243, /* DiagArray291 */ 806, 0, R"()")
DIAG_ENTRY(anonymous_143 /* empty-translation-unit */, 6259, /* DiagArray292 */ 808, 0, R"()")
DIAG_ENTRY(anonymous_212 /* encode-type */, 6282, /* DiagArray293 */ 810, 0, R"()")
DIAG_ENTRY(anonymous_61 /* endif-labels */, 6294, 0, /* DiagSubGroup294 */ 253, R"()")
DIAG_ENTRY(EnumCompare /* enum-compare */, 6307, /* DiagArray295 */ 812, /* DiagSubGroup295 */ 255, R"()")
DIAG_ENTRY(EnumCompareConditional /* enum-compare-conditional */, 6320, /* DiagArray296 */ 814, /* DiagSubGroup296 */ 258, R"()")
DIAG_ENTRY(EnumCompareSwitch /* enum-compare-switch */, 6345, /* DiagArray297 */ 816, 0, R"()")
DIAG_ENTRY(anonymous_81 /* enum-constexpr-conversion */, 6365, /* DiagArray298 */ 818, 0, R"()")
DIAG_ENTRY(EnumConversion /* enum-conversion */, 6391, /* DiagArray299 */ 820, /* DiagSubGroup299 */ 260, R"()")
DIAG_ENTRY(EnumEnumConversion /* enum-enum-conversion */, 6407, /* DiagArray300 */ 822, /* DiagSubGroup300 */ 264, R"()")
DIAG_ENTRY(EnumFloatConversion /* enum-float-conversion */, 6428, /* DiagArray301 */ 824, /* DiagSubGroup301 */ 266, R"()")
DIAG_ENTRY(EnumTooLarge /* enum-too-large */, 6450, /* DiagArray302 */ 826, 0, R"()")
DIAG_ENTRY(Exceptions /* exceptions */, 6465, /* DiagArray303 */ 829, 0, R"()")
DIAG_ENTRY(ExcessInitializers /* excess-initializers */, 6476, /* DiagArray304 */ 833, 0, R"()")
DIAG_ENTRY(anonymous_162 /* excessive-regsave */, 6496, /* DiagArray305 */ 838, 0, R"()")
DIAG_ENTRY(ExitTimeDestructors /* exit-time-destructors */, 6514, /* DiagArray306 */ 840, 0, R"()")
DIAG_ENTRY(ExpansionToDefined /* expansion-to-defined */, 6536, /* DiagArray307 */ 842, 0, R"()")
DIAG_ENTRY(anonymous_300 /* experimental-header-units */, 6557, /* DiagArray308 */ 845, 0, R"()")
DIAG_ENTRY(ExplicitInitializeCall /* explicit-initialize-call */, 6583, /* DiagArray309 */ 847, 0, R"()")
DIAG_ENTRY(anonymous_246 /* explicit-ownership-type */, 6608, /* DiagArray310 */ 850, 0, R"()")
DIAG_ENTRY(ExportUnnamed /* export-unnamed */, 6632, 0, 0, R"()")
DIAG_ENTRY(ExternCCompat /* extern-c-compat */, 6647, /* DiagArray312 */ 852, 0, R"()")
DIAG_ENTRY(anonymous_241 /* extern-initializer */, 6663, /* DiagArray313 */ 854, 0, R"()")
DIAG_ENTRY(Extra /* extra */, 6682, /* DiagArray314 */ 856, /* DiagSubGroup314 */ 268, R"()")
DIAG_ENTRY(anonymous_255 /* extra-qualification */, 6688, /* DiagArray315 */ 858, 0, R"()")
DIAG_ENTRY(ExtraSemi /* extra-semi */, 6708, /* DiagArray316 */ 860, /* DiagSubGroup316 */ 284, R"()")
DIAG_ENTRY(ExtraSemiStmt /* extra-semi-stmt */, 6719, /* DiagArray317 */ 863, /* DiagSubGroup317 */ 287, R"()")
DIAG_ENTRY(ExtraTokens /* extra-tokens */, 6735, /* DiagArray318 */ 865, 0, R"()")
DIAG_ENTRY(ExtractAPIMisuse /* extractapi-misuse */, 6748, /* DiagArray319 */ 868, 0, R"()")
DIAG_ENTRY(FinalDtorNonFinalClass /* final-dtor-non-final-class */, 6766, /* DiagArray320 */ 870, 0, R"()")
DIAG_ENTRY(FinalMacro /* final-macro */, 6793, /* DiagArray321 */ 872, 0, R"()")
DIAG_ENTRY(anonymous_145 /* fixed-enum-extension */, 6805, /* DiagArray322 */ 874, 0, R"()")
DIAG_ENTRY(anonymous_80 /* fixed-point-overflow */, 6826, /* DiagArray323 */ 876, 0, R"()")
DIAG_ENTRY(FlagEnum /* flag-enum */, 6847, /* DiagArray324 */ 878, 0, R"()")
DIAG_ENTRY(FlexibleArrayExtensions /* flexible-array-extensions */, 6857, /* DiagArray325 */ 880, 0, R"()")
DIAG_ENTRY(FloatConversion /* float-conversion */, 6883, /* DiagArray326 */ 883, /* DiagSubGroup326 */ 289, R"()")
DIAG_ENTRY(anonymous_248 /* float-equal */, 6900, /* DiagArray327 */ 885, 0, R"()")
DIAG_ENTRY(FloatOverflowConversion /* float-overflow-conversion */, 6912, /* DiagArray328 */ 887, 0, R"()")
DIAG_ENTRY(FloatZeroConversion /* float-zero-conversion */, 6938, /* DiagArray329 */ 890, 0, R"()")
DIAG_ENTRY(ForLoopAnalysis /* for-loop-analysis */, 6960, /* DiagArray330 */ 892, 0, R"()")
DIAG_ENTRY(Format /* format */, 6978, /* DiagArray331 */ 895, /* DiagSubGroup331 */ 292, R"()")
DIAG_ENTRY(FormatExtraArgs /* format-extra-args */, 6985, /* DiagArray332 */ 923, 0, R"()")
DIAG_ENTRY(FormatInsufficientArgs /* format-insufficient-args */, 7003, /* DiagArray333 */ 925, 0, R"()")
DIAG_ENTRY(FormatInvalidSpecifier /* format-invalid-specifier */, 7028, /* DiagArray334 */ 927, 0, R"()")
DIAG_ENTRY(FormatNonStandard /* format-non-iso */, 7053, /* DiagArray335 */ 929, 0, R"()")
DIAG_ENTRY(FormatNonLiteral /* format-nonliteral */, 7068, /* DiagArray336 */ 933, 0, R"()")
DIAG_ENTRY(FormatOverflow /* format-overflow */, 7086, /* DiagArray337 */ 935, /* DiagSubGroup337 */ 302, R"()")
DIAG_ENTRY(FormatOverflowNonKprintf /* format-overflow-non-kprintf */, 7102, /* DiagArray338 */ 937, 0, R"()")
DIAG_ENTRY(FormatPedantic /* format-pedantic */, 7130, /* DiagArray339 */ 939, 0, R"()")
DIAG_ENTRY(FormatSecurity /* format-security */, 7146, /* DiagArray340 */ 942, 0, R"()")
DIAG_ENTRY(FormatSignedness /* format-signedness */, 7162, /* DiagArray341 */ 944, 0, R"()")
DIAG_ENTRY(FormatTruncation /* format-truncation */, 7180, /* DiagArray342 */ 946, /* DiagSubGroup342 */ 304, R"()")
DIAG_ENTRY(FormatTruncationNonKprintf /* format-truncation-non-kprintf */, 7198, /* DiagArray343 */ 948, 0, R"()")
DIAG_ENTRY(FormatTypeConfusion /* format-type-confusion */, 7228, /* DiagArray344 */ 950, 0, R"()")
DIAG_ENTRY(FormatY2K /* format-y2k */, 7250, 0, 0, R"()")
DIAG_ENTRY(FormatZeroLength /* format-zero-length */, 7261, /* DiagArray346 */ 952, 0, R"()")
DIAG_ENTRY(Format2 /* format=2 */, 7280, 0, /* DiagSubGroup347 */ 306, R"()")
DIAG_ENTRY(FortifySource /* fortify-source */, 7289, /* DiagArray348 */ 954, /* DiagSubGroup348 */ 310, R"()")
DIAG_ENTRY(FourByteMultiChar /* four-char-constants */, 7304, /* DiagArray349 */ 959, 0, R"()")
DIAG_ENTRY(FrameAddress /* frame-address */, 7324, /* DiagArray350 */ 961, 0, R"()")
DIAG_ENTRY(BackendFrameLargerThan /* frame-larger-than */, 7338, /* DiagArray351 */ 963, 0, R"(More fine grained information about the stack layout is available by adding the
`-Rpass-analysis=stack-frame-layout` command-line flag to the compiler
invocation.

The diagnostic information can be saved to a file in a machine readable format,
like YAML by adding the `-foptimization-record-file=<file>` command-line flag.

Results can be filtered by function name by passing
`-mllvm -filter-print-funcs=foo`, where `foo` is the target function's name.

   .. code-block:: console

      clang -c a.cpp -Rpass-analysis=stack-frame-layout -mllvm -filter-print-funcs=foo

   .. code-block:: console

      clang -c a.cpp -Rpass-analysis=stack-frame-layout -foptimization-record-file=<file>)")
DIAG_ENTRY(anonymous_77 /* frame-larger-than= */, 7356, 0, /* DiagSubGroup352 */ 313, R"()")
DIAG_ENTRY(FrameworkIncludePrivateFromPublic /* framework-include-private-from-public */, 7375, /* DiagArray353 */ 966, 0, R"()")
DIAG_ENTRY(FreeNonHeapObject /* free-nonheap-object */, 7413, /* DiagArray354 */ 968, 0, R"()")
DIAG_ENTRY(anonymous_189 /* friend-enum */, 7433, /* DiagArray355 */ 970, 0, R"()")
DIAG_ENTRY(FunctionDefInObjCContainer /* function-def-in-objc-container */, 7445, /* DiagArray356 */ 972, 0, R"()")
DIAG_ENTRY(FunctionMultiVersioning /* function-multiversion */, 7476, /* DiagArray357 */ 974, /* DiagSubGroup357 */ 315, R"()")
DIAG_ENTRY(FUseLdPath /* fuse-ld-path */, 7498, /* DiagArray358 */ 979, 0, R"()")
DIAG_ENTRY(FutureAttrs /* future-attribute-extensions */, 7511, 0, /* DiagSubGroup359 */ 317, R"()")
DIAG_ENTRY(FutureCompat /* future-compat */, 7539, 0, 0, R"()")
DIAG_ENTRY(GccCompat /* gcc-compat */, 7553, /* DiagArray361 */ 981, 0, R"()")
DIAG_ENTRY(anonymous_149 /* generic-type-extension */, 7564, /* DiagArray362 */ 993, 0, R"()")
DIAG_ENTRY(GlobalConstructors /* global-constructors */, 7587, /* DiagArray363 */ 995, 0, R"(Emit a warning for each variable declaration that generates code run at startup.)")
DIAG_ENTRY(GlobalISel /* global-isel */, 7607, /* DiagArray364 */ 998, 0, R"()")
DIAG_ENTRY(GNU /* gnu */, 7619, 0, /* DiagSubGroup365 */ 322, R"()")
DIAG_ENTRY(GNUAlignofExpression /* gnu-alignof-expression */, 7623, /* DiagArray366 */ 1001, 0, R"()")
DIAG_ENTRY(GNUAnonymousStruct /* gnu-anonymous-struct */, 7646, /* DiagArray367 */ 1003, 0, R"()")
DIAG_ENTRY(anonymous_258 /* gnu-array-member-paren-init */, 7667, /* DiagArray368 */ 1005, 0, R"()")
DIAG_ENTRY(GNUAutoType /* gnu-auto-type */, 7695, /* DiagArray369 */ 1007, 0, R"()")
DIAG_ENTRY(GNUBinaryLiteral /* gnu-binary-literal */, 7709, 0, 0, R"()")
DIAG_ENTRY(GNUCaseRange /* gnu-case-range */, 7728, /* DiagArray371 */ 1009, 0, R"()")
DIAG_ENTRY(GNUComplexInteger /* gnu-complex-integer */, 7743, /* DiagArray372 */ 1011, 0, R"()")
DIAG_ENTRY(GNUCompoundLiteralInitializer /* gnu-compound-literal-initializer */, 7763, /* DiagArray373 */ 1013, 0, R"()")
DIAG_ENTRY(GNUConditionalOmittedOperand /* gnu-conditional-omitted-operand */, 7796, /* DiagArray374 */ 1015, 0, R"()")
DIAG_ENTRY(GNUDesignator /* gnu-designator */, 7828, /* DiagArray375 */ 1017, 0, R"()")
DIAG_ENTRY(anonymous_1 /* gnu-empty-initializer */, 7843, 0, 0, R"()")
DIAG_ENTRY(GNUEmptyStruct /* gnu-empty-struct */, 7865, /* DiagArray377 */ 1021, 0, R"()")
DIAG_ENTRY(GNUFlexibleArrayInitializer /* gnu-flexible-array-initializer */, 7882, /* DiagArray378 */ 1025, 0, R"()")
DIAG_ENTRY(GNUFlexibleArrayUnionMember /* gnu-flexible-array-union-member */, 7913, /* DiagArray379 */ 1027, 0, R"()")
DIAG_ENTRY(GNUFoldingConstant /* gnu-folding-constant */, 7945, /* DiagArray380 */ 1029, 0, R"()")
DIAG_ENTRY(GNUImaginaryConstant /* gnu-imaginary-constant */, 7966, /* DiagArray381 */ 1033, 0, R"()")
DIAG_ENTRY(GNUIncludeNext /* gnu-include-next */, 7989, /* DiagArray382 */ 1035, 0, R"()")
DIAG_ENTRY(anonymous_213 /* gnu-inline-cpp-without-extern */, 8006, /* DiagArray383 */ 1037, 0, R"()")
DIAG_ENTRY(GNULabelsAsValue /* gnu-label-as-value */, 8036, /* DiagArray384 */ 1039, 0, R"()")
DIAG_ENTRY(GNULineMarker /* gnu-line-marker */, 8055, /* DiagArray385 */ 1042, 0, R"()")
DIAG_ENTRY(GNUNullPointerArithmetic /* gnu-null-pointer-arithmetic */, 8071, /* DiagArray386 */ 1044, 0, R"()")
DIAG_ENTRY(GNUOffsetofExtensions /* gnu-offsetof-extensions */, 8099, 0, 0, R"()")
DIAG_ENTRY(GNUPointerArith /* gnu-pointer-arith */, 8123, /* DiagArray388 */ 1046, 0, R"()")
DIAG_ENTRY(GNURedeclaredEnum /* gnu-redeclared-enum */, 8141, /* DiagArray389 */ 1050, 0, R"()")
DIAG_ENTRY(GNUStatementExpression /* gnu-statement-expression */, 8161, /* DiagArray390 */ 1052, /* DiagSubGroup390 */ 354, R"()")
DIAG_ENTRY(GNUStatementExpressionFromMacroExpansion /* gnu-statement-expression-from-macro-expansion */, 8186, /* DiagArray391 */ 1054, 0, R"()")
DIAG_ENTRY(GNUStaticFloatInit /* gnu-static-float-init */, 8232, /* DiagArray392 */ 1056, 0, R"()")
DIAG_ENTRY(GNUStringLiteralOperatorTemplate /* gnu-string-literal-operator-template */, 8254, /* DiagArray393 */ 1058, 0, R"()")
DIAG_ENTRY(GNUUnionCast /* gnu-union-cast */, 8291, /* DiagArray394 */ 1060, 0, R"()")
DIAG_ENTRY(GNUVariableSizedTypeNotAtEnd /* gnu-variable-sized-type-not-at-end */, 8306, /* DiagArray395 */ 1062, 0, R"()")
DIAG_ENTRY(GNUZeroLineDirective /* gnu-zero-line-directive */, 8341, /* DiagArray396 */ 1064, 0, R"()")
DIAG_ENTRY(GNUZeroVariadicMacroArguments /* gnu-zero-variadic-macro-arguments */, 8365, /* DiagArray397 */ 1066, 0, R"()")
DIAG_ENTRY(anonymous_277 /* gpu-maybe-wrong-side */, 8399, /* DiagArray398 */ 1068, 0, R"()")
DIAG_ENTRY(anonymous_142 /* header-guard */, 8420, /* DiagArray399 */ 1070, 0, R"()")
DIAG_ENTRY(HeaderHygiene /* header-hygiene */, 8433, /* DiagArray400 */ 1072, 0, R"()")
DIAG_ENTRY(anonymous_84 /* higher-precision-fp */, 8448, /* DiagArray401 */ 1074, 0, R"()")
DIAG_ENTRY(HIPOpenMPOffloading /* hip-omp-target-directives */, 8468, /* DiagArray402 */ 1076, 0, R"()")
DIAG_ENTRY(HIPOnly /* hip-only */, 8494, /* DiagArray403 */ 1078, 0, R"()")
DIAG_ENTRY(HLSLExtension /* hlsl-extensions */, 8503, /* DiagArray404 */ 1080, 0, R"()")
DIAG_ENTRY(anonymous_272 /* idiomatic-parentheses */, 8519, /* DiagArray405 */ 1082, 0, R"()")
DIAG_ENTRY(IgnoredAttributes /* ignored-attributes */, 8541, /* DiagArray406 */ 1084, 0, R"()")
DIAG_ENTRY(anonymous_216 /* ignored-availability-without-sdk-settings */, 8560, /* DiagArray407 */ 1173, 0, R"()")
DIAG_ENTRY(IgnoredGCH /* ignored-gch */, 8602, /* DiagArray408 */ 1175, 0, R"()")
DIAG_ENTRY(IgnoredOptimizationArgument /* ignored-optimization-argument */, 8614, /* DiagArray409 */ 1178, 0, R"()")
DIAG_ENTRY(IgnoredPragmaIntrinsic /* ignored-pragma-intrinsic */, 8644, /* DiagArray410 */ 1181, 0, R"()")
DIAG_ENTRY(IgnoredPragmaOptimize /* ignored-pragma-optimize */, 8669, 0, 0, R"()")
DIAG_ENTRY(IgnoredPragmas /* ignored-pragmas */, 8693, /* DiagArray412 */ 1183, /* DiagSubGroup412 */ 356, R"()")
DIAG_ENTRY(IgnoredQualifiers /* ignored-qualifiers */, 8709, /* DiagArray413 */ 1233, /* DiagSubGroup413 */ 359, R"()")
DIAG_ENTRY(IgnoredReferenceQualifiers /* ignored-reference-qualifiers */, 8728, /* DiagArray414 */ 1238, 0, R"()")
DIAG_ENTRY(Implicit /* implicit */, 8757, 0, /* DiagSubGroup415 */ 361, R"()")
DIAG_ENTRY(ImplicitAtomic /* implicit-atomic-properties */, 8766, /* DiagArray416 */ 1240, 0, R"()")
DIAG_ENTRY(ImplicitConstIntFloatConversion /* implicit-const-int-float-conversion */, 8793, /* DiagArray417 */ 1243, 0, R"()")
DIAG_ENTRY(ImplicitConversionFloatingPointToBool /* implicit-conversion-floating-point-to-bool */, 8829, /* DiagArray418 */ 1245, 0, R"()")
DIAG_ENTRY(anonymous_288 /* implicit-exception-spec-mismatch */, 8872, /* DiagArray419 */ 1247, 0, R"()")
DIAG_ENTRY(ImplicitFallthrough /* implicit-fallthrough */, 8905, /* DiagArray420 */ 1249, /* DiagSubGroup420 */ 364, R"()")
DIAG_ENTRY(ImplicitFallthroughPerFunction /* implicit-fallthrough-per-function */, 8926, /* DiagArray421 */ 1251, 0, R"()")
DIAG_ENTRY(ImplicitFixedPointConversion /* implicit-fixed-point-conversion */, 8960, /* DiagArray422 */ 1253, 0, R"()")
DIAG_ENTRY(ImplicitFloatConversion /* implicit-float-conversion */, 8992, /* DiagArray423 */ 1255, /* DiagSubGroup423 */ 366, R"()")
DIAG_ENTRY(ImplicitFunctionDeclare /* implicit-function-declaration */, 9018, /* DiagArray424 */ 1258, 0, R"()")
DIAG_ENTRY(ImplicitInt /* implicit-int */, 9048, /* DiagArray425 */ 1264, 0, R"()")
DIAG_ENTRY(ImplicitIntConversion /* implicit-int-conversion */, 9061, /* DiagArray426 */ 1268, /* DiagSubGroup426 */ 369, R"()")
DIAG_ENTRY(ImplicitIntFloatConversion /* implicit-int-float-conversion */, 9085, /* DiagArray427 */ 1271, /* DiagSubGroup427 */ 372, R"()")
DIAG_ENTRY(anonymous_184 /* implicit-retain-self */, 9115, /* DiagArray428 */ 1273, 0, R"()")
DIAG_ENTRY(ImplicitlyUnsignedLiteral /* implicitly-unsigned-literal */, 9136, /* DiagArray429 */ 1275, 0, R"()")
DIAG_ENTRY(anonymous_30 /* import */, 9164, 0, 0, R"()")
DIAG_ENTRY(anonymous_135 /* import-preprocessor-directive-pedantic */, 9171, /* DiagArray431 */ 1277, 0, R"()")
DIAG_ENTRY(anonymous_285 /* inaccessible-base */, 9210, /* DiagArray432 */ 1279, 0, R"()")
DIAG_ENTRY(anonymous_141 /* include-angled-in-module-purview */, 9228, /* DiagArray433 */ 1281, 0, R"()")
DIAG_ENTRY(anonymous_125 /* include-next-absolute-path */, 9261, /* DiagArray434 */ 1283, 0, R"()")
DIAG_ENTRY(anonymous_124 /* include-next-outside-header */, 9288, /* DiagArray435 */ 1285, 0, R"()")
DIAG_ENTRY(IncompatibleExceptionSpec /* incompatible-exception-spec */, 9316, /* DiagArray436 */ 1287, 0, R"()")
DIAG_ENTRY(IncompatibleFunctionPointerTypes /* incompatible-function-pointer-types */, 9344, /* DiagArray437 */ 1290, 0, R"()")
DIAG_ENTRY(anonymous_274 /* incompatible-function-pointer-types-strict */, 9380, /* DiagArray438 */ 1292, 0, R"()")
DIAG_ENTRY(anonymous_167 /* incompatible-library-redeclaration */, 9423, /* DiagArray439 */ 1294, 0, R"()")
DIAG_ENTRY(IncompatibleMSPragmaSection /* incompatible-ms-pragma-section */, 9458, /* DiagArray440 */ 1296, 0, R"()")
DIAG_ENTRY(IncompatibleMSStruct /* incompatible-ms-struct */, 9489, /* DiagArray441 */ 1298, 0, R"()")
DIAG_ENTRY(IncompatiblePointerTypes /* incompatible-pointer-types */, 9512, /* DiagArray442 */ 1301, /* DiagSubGroup442 */ 374, R"()")
DIAG_ENTRY(IncompatiblePointerTypesDiscardsQualifiers /* incompatible-pointer-types-discards-qualifiers */, 9539, /* DiagArray443 */ 1303, 0, R"()")
DIAG_ENTRY(anonymous_174 /* incompatible-property-type */, 9586, /* DiagArray444 */ 1307, 0, R"()")
DIAG_ENTRY(anonymous_97 /* incompatible-sysroot */, 9613, /* DiagArray445 */ 1309, 0, R"()")
DIAG_ENTRY(IncompleteFrameworkModuleDeclaration /* incomplete-framework-module-declaration */, 9634, /* DiagArray446 */ 1311, 0, R"()")
DIAG_ENTRY(anonymous_176 /* incomplete-implementation */, 9674, /* DiagArray447 */ 1313, 0, R"()")
DIAG_ENTRY(IncompleteModule /* incomplete-module */, 9700, 0, /* DiagSubGroup448 */ 377, R"()")
DIAG_ENTRY(anonymous_166 /* incomplete-setjmp-declaration */, 9718, /* DiagArray449 */ 1315, 0, R"()")
DIAG_ENTRY(IncompleteUmbrella /* incomplete-umbrella */, 9748, /* DiagArray450 */ 1317, 0, R"()")
DIAG_ENTRY(MicrosoftInconsistentDllImport /* inconsistent-dllimport */, 9768, /* DiagArray451 */ 1321, 0, R"()")
DIAG_ENTRY(CXX11WarnInconsistentOverrideDestructor /* inconsistent-missing-destructor-override */, 9791, /* DiagArray452 */ 1324, 0, R"()")
DIAG_ENTRY(CXX11WarnInconsistentOverrideMethod /* inconsistent-missing-override */, 9832, /* DiagArray453 */ 1326, 0, R"()")
DIAG_ENTRY(IncrementBool /* increment-bool */, 9862, /* DiagArray454 */ 1328, /* DiagSubGroup454 */ 380, R"()")
DIAG_ENTRY(InfiniteRecursion /* infinite-recursion */, 9877, /* DiagArray455 */ 1330, 0, R"()")
DIAG_ENTRY(anonymous_31 /* init-self */, 9896, 0, 0, R"()")
DIAG_ENTRY(InitializerOverrides /* initializer-overrides */, 9906, /* DiagArray457 */ 1332, 0, R"()")
DIAG_ENTRY(anonymous_193 /* injected-class-name */, 9928, /* DiagArray458 */ 1336, 0, R"()")
DIAG_ENTRY(anonymous_32 /* inline */, 9948, 0, 0, R"()")
DIAG_ENTRY(BackendInlineAsm /* inline-asm */, 9955, /* DiagArray460 */ 1338, 0, R"()")
DIAG_ENTRY(InlineNamespaceReopenedNoninline /* inline-namespace-reopened-noninline */, 9966, /* DiagArray461 */ 1340, 0, R"()")
DIAG_ENTRY(anonymous_286 /* inline-new-delete */, 10002, /* DiagArray462 */ 1342, 0, R"()")
DIAG_ENTRY(InstallAPIViolation /* installapi-violation */, 10020, /* DiagArray463 */ 1344, 0, R"()")
DIAG_ENTRY(anonymous_230 /* instantiation-after-specialization */, 10041, /* DiagArray464 */ 1358, 0, R"()")
DIAG_ENTRY(IntConversion /* int-conversion */, 10076, /* DiagArray465 */ 1360, 0, R"()")
DIAG_ENTRY(anonymous_66 /* int-conversions */, 10091, 0, /* DiagSubGroup466 */ 382, R"()")
DIAG_ENTRY(IntInBoolContext /* int-in-bool-context */, 10107, /* DiagArray467 */ 1363, 0, R"()")
DIAG_ENTRY(IntToPointerCast /* int-to-pointer-cast */, 10127, /* DiagArray468 */ 1366, /* DiagSubGroup468 */ 384, R"()")
DIAG_ENTRY(IntToVoidPointerCast /* int-to-void-pointer-cast */, 10147, /* DiagArray469 */ 1368, 0, R"()")
DIAG_ENTRY(anonymous_79 /* integer-overflow */, 10172, /* DiagArray470 */ 1370, 0, R"()")
DIAG_ENTRY(InvalidCommandLineArgument /* invalid-command-line-argument */, 10189, /* DiagArray471 */ 1372, /* DiagSubGroup471 */ 386, R"()")
DIAG_ENTRY(anonymous_196 /* invalid-constexpr */, 10219, /* DiagArray472 */ 1384, 0, R"()")
DIAG_ENTRY(anonymous_89 /* invalid-feature-combination */, 10237, /* DiagArray473 */ 1386, 0, R"()")
DIAG_ENTRY(ObjCInvalidIBOutletProperty /* invalid-iboutlet */, 10265, /* DiagArray474 */ 1388, 0, R"()")
DIAG_ENTRY(anonymous_210 /* invalid-initializer-from-system-header */, 10282, /* DiagArray475 */ 1391, 0, R"()")
DIAG_ENTRY(InvalidIOSDeploymentTarget /* invalid-ios-deployment-target */, 10321, /* DiagArray476 */ 1393, 0, R"()")
DIAG_ENTRY(anonymous_222 /* invalid-no-builtin-names */, 10351, /* DiagArray477 */ 1395, 0, R"()")
DIAG_ENTRY(InvalidNoreturn /* invalid-noreturn */, 10376, /* DiagArray478 */ 1397, 0, R"()")
DIAG_ENTRY(InvalidOffsetof /* invalid-offsetof */, 10393, /* DiagArray479 */ 1400, 0, R"()")
DIAG_ENTRY(InvalidOrNonExistentDirectory /* invalid-or-nonexistent-directory */, 10410, /* DiagArray480 */ 1403, 0, R"()")
DIAG_ENTRY(anonymous_228 /* invalid-partial-specialization */, 10443, /* DiagArray481 */ 1405, 0, R"()")
DIAG_ENTRY(anonymous_33 /* invalid-pch */, 10474, 0, 0, R"()")
DIAG_ENTRY(InvalidPPToken /* invalid-pp-token */, 10486, /* DiagArray483 */ 1407, 0, R"()")
DIAG_ENTRY(InvalidSourceEncoding /* invalid-source-encoding */, 10503, /* DiagArray484 */ 1410, 0, R"()")
DIAG_ENTRY(anonymous_187 /* invalid-static-assert-message */, 10527, /* DiagArray485 */ 1413, 0, R"()")
DIAG_ENTRY(anonymous_138 /* invalid-token-paste */, 10557, /* DiagArray486 */ 1415, 0, R"()")
DIAG_ENTRY(anonymous_123 /* invalid-unevaluated-string */, 10577, /* DiagArray487 */ 1417, 0, R"()")
DIAG_ENTRY(anonymous_116 /* invalid-utf8 */, 10604, /* DiagArray488 */ 1419, 0, R"()")
DIAG_ENTRY(anonymous_269 /* jump-seh-finally */, 10617, /* DiagArray489 */ 1421, 0, R"()")
DIAG_ENTRY(KeywordCompat /* keyword-compat */, 10634, /* DiagArray490 */ 1423, 0, R"()")
DIAG_ENTRY(KeywordAsMacro /* keyword-macro */, 10649, /* DiagArray491 */ 1425, 0, R"()")
DIAG_ENTRY(KNRPromotedParameter /* knr-promoted-parameter */, 10663, /* DiagArray492 */ 1427, 0, R"()")
DIAG_ENTRY(anonymous_115 /* language-extension-token */, 10686, /* DiagArray493 */ 1429, 0, R"()")
DIAG_ENTRY(LargeByValueCopy /* large-by-value-copy */, 10711, /* DiagArray494 */ 1431, 0, R"()")
DIAG_ENTRY(LibLTO /* liblto */, 10731, 0, 0, R"()")
DIAG_ENTRY(LinkerWarnings /* linker-warnings */, 10738, /* DiagArray496 */ 1434, 0, R"()")
DIAG_ENTRY(LiteralConversion /* literal-conversion */, 10754, /* DiagArray497 */ 1436, 0, R"()")
DIAG_ENTRY(LiteralRange /* literal-range */, 10773, /* DiagArray498 */ 1439, 0, R"()")
DIAG_ENTRY(LocalTypeTemplateArgs /* local-type-template-args */, 10787, /* DiagArray499 */ 1443, /* DiagSubGroup499 */ 388, R"()")
DIAG_ENTRY(LogicalNotParentheses /* logical-not-parentheses */, 10812, /* DiagArray500 */ 1445, 0, R"()")
DIAG_ENTRY(LogicalOpParentheses /* logical-op-parentheses */, 10836, /* DiagArray501 */ 1447, 0, R"()")
DIAG_ENTRY(LongLong /* long-long */, 10859, /* DiagArray502 */ 1449, /* DiagSubGroup502 */ 390, R"()")
DIAG_ENTRY(LoopAnalysis /* loop-analysis */, 10869, 0, /* DiagSubGroup503 */ 392, R"()")
DIAG_ENTRY(MacroRedefined /* macro-redefined */, 10883, /* DiagArray504 */ 1451, 0, R"()")
DIAG_ENTRY(Main /* main */, 10899, /* DiagArray505 */ 1453, 0, R"()")
DIAG_ENTRY(MainReturnType /* main-return-type */, 10904, /* DiagArray506 */ 1461, 0, R"()")
DIAG_ENTRY(MalformedWarningCheck /* malformed-warning-check */, 10921, /* DiagArray507 */ 1463, 0, R"()")
DIAG_ENTRY(anonymous_243 /* many-braces-around-scalar-init */, 10945, /* DiagArray508 */ 1465, 0, R"()")
DIAG_ENTRY(anonymous_120 /* mathematical-notation-identifier-extension */, 10976, /* DiagArray509 */ 1467, 0, R"()")
DIAG_ENTRY(MaxTokens /* max-tokens */, 11019, /* DiagArray510 */ 1469, 0, R"(The warning is issued if the number of pre-processor tokens exceeds
the token limit, which can be set in three ways:

1. As a limit at a specific point in a file, using the ``clang max_tokens_here``
   pragma:

   .. code-block:: c++

      #pragma clang max_tokens_here 1234

2. As a per-translation unit limit, using the ``-fmax-tokens=`` command-line
   flag:

   .. code-block:: console

      clang -c a.cpp -fmax-tokens=1234

3. As a per-translation unit limit using the ``clang max_tokens_total`` pragma,
   which works like and overrides the ``-fmax-tokens=`` flag:

   .. code-block:: c++

      #pragma clang max_tokens_total 1234

These limits can be helpful in limiting code growth through included files.

Setting a token limit of zero means no limit.

Note that the warning is disabled by default, so -Wmax-tokens must be used
in addition with the pragmas or -fmax-tokens flag to get any warnings.)")
DIAG_ENTRY(MaxUnsignedZero /* max-unsigned-zero */, 11030, /* DiagArray511 */ 1472, 0, R"()")
DIAG_ENTRY(MemsetTransposedArgs /* memset-transposed-args */, 11048, /* DiagArray512 */ 1474, 0, R"()")
DIAG_ENTRY(anonymous_169 /* memsize-comparison */, 11071, /* DiagArray513 */ 1476, 0, R"()")
DIAG_ENTRY(MethodSignatures /* method-signatures */, 11090, /* DiagArray514 */ 1478, 0, R"()")
DIAG_ENTRY(Microsoft /* microsoft */, 11108, 0, /* DiagSubGroup515 */ 395, R"()")
DIAG_ENTRY(MicrosoftAbstract /* microsoft-abstract */, 11118, /* DiagArray516 */ 1481, 0, R"()")
DIAG_ENTRY(MicrosoftAnonTag /* microsoft-anon-tag */, 11137, /* DiagArray517 */ 1483, 0, R"()")
DIAG_ENTRY(MicrosoftCast /* microsoft-cast */, 11156, /* DiagArray518 */ 1486, 0, R"()")
DIAG_ENTRY(MicrosoftCharize /* microsoft-charize */, 11171, /* DiagArray519 */ 1489, 0, R"()")
DIAG_ENTRY(MicrosoftCommentPaste /* microsoft-comment-paste */, 11189, /* DiagArray520 */ 1491, 0, R"()")
DIAG_ENTRY(MicrosoftConstInit /* microsoft-const-init */, 11213, /* DiagArray521 */ 1493, 0, R"()")
DIAG_ENTRY(MicrosoftCppMacro /* microsoft-cpp-macro */, 11234, /* DiagArray522 */ 1495, 0, R"()")
DIAG_ENTRY(MicrosoftDefaultArgRedefinition /* microsoft-default-arg-redefinition */, 11254, /* DiagArray523 */ 1497, 0, R"()")
DIAG_ENTRY(MicrosoftDrectveSection /* microsoft-drectve-section */, 11289, /* DiagArray524 */ 1499, 0, R"()")
DIAG_ENTRY(MicrosoftEndOfFile /* microsoft-end-of-file */, 11315, /* DiagArray525 */ 1501, 0, R"()")
DIAG_ENTRY(MicrosoftEnumForwardReference /* microsoft-enum-forward-reference */, 11337, /* DiagArray526 */ 1503, 0, R"()")
DIAG_ENTRY(MicrosoftEnumValue /* microsoft-enum-value */, 11370, /* DiagArray527 */ 1505, 0, R"()")
DIAG_ENTRY(MicrosoftExceptionSpec /* microsoft-exception-spec */, 11391, /* DiagArray528 */ 1507, 0, R"()")
DIAG_ENTRY(anonymous_148 /* microsoft-exists */, 11416, /* DiagArray529 */ 1513, 0, R"()")
DIAG_ENTRY(MicrosoftExplicitConstructorCall /* microsoft-explicit-constructor-call */, 11433, /* DiagArray530 */ 1515, 0, R"()")
DIAG_ENTRY(MicrosoftExtraQualification /* microsoft-extra-qualification */, 11469, /* DiagArray531 */ 1517, 0, R"()")
DIAG_ENTRY(MicrosoftFixedEnum /* microsoft-fixed-enum */, 11499, /* DiagArray532 */ 1519, 0, R"()")
DIAG_ENTRY(MicrosoftFlexibleArray /* microsoft-flexible-array */, 11520, /* DiagArray533 */ 1521, 0, R"()")
DIAG_ENTRY(MicrosoftGoto /* microsoft-goto */, 11545, /* DiagArray534 */ 1524, 0, R"()")
DIAG_ENTRY(MicrosoftInaccessibleBase /* microsoft-inaccessible-base */, 11560, /* DiagArray535 */ 1526, 0, R"()")
DIAG_ENTRY(MicrosoftInclude /* microsoft-include */, 11588, /* DiagArray536 */ 1528, 0, R"()")
DIAG_ENTRY(MicrosoftInitFromPredefined /* microsoft-init-from-predefined */, 11606, /* DiagArray537 */ 1530, 0, R"()")
DIAG_ENTRY(MicrosoftMutableReference /* microsoft-mutable-reference */, 11637, /* DiagArray538 */ 1532, 0, R"()")
DIAG_ENTRY(MicrosoftPureDefinition /* microsoft-pure-definition */, 11665, /* DiagArray539 */ 1534, 0, R"()")
DIAG_ENTRY(MicrosoftRedeclareStatic /* microsoft-redeclare-static */, 11691, /* DiagArray540 */ 1536, 0, R"()")
DIAG_ENTRY(MicrosoftSealed /* microsoft-sealed */, 11718, /* DiagArray541 */ 1538, 0, R"()")
DIAG_ENTRY(MicrosoftStaticAssert /* microsoft-static-assert */, 11735, /* DiagArray542 */ 1540, 0, R"()")
DIAG_ENTRY(MicrosoftStringLiteralFromPredefined /* microsoft-string-literal-from-predefined */, 11759, /* DiagArray543 */ 1542, 0, R"()")
DIAG_ENTRY(MicrosoftTemplate /* microsoft-template */, 11800, /* DiagArray544 */ 1544, /* DiagSubGroup544 */ 428, R"()")
DIAG_ENTRY(MicrosoftTemplateShadow /* microsoft-template-shadow */, 11819, /* DiagArray545 */ 1555, 0, R"()")
DIAG_ENTRY(MicrosoftUnionMemberReference /* microsoft-union-member-reference */, 11845, /* DiagArray546 */ 1557, 0, R"()")
DIAG_ENTRY(MicrosoftUnqualifiedFriend /* microsoft-unqualified-friend */, 11878, /* DiagArray547 */ 1559, 0, R"()")
DIAG_ENTRY(MicrosoftUsingDecl /* microsoft-using-decl */, 11907, /* DiagArray548 */ 1561, 0, R"()")
DIAG_ENTRY(MicrosoftVoidPseudoDtor /* microsoft-void-pseudo-dtor */, 11928, /* DiagArray549 */ 1563, 0, R"()")
DIAG_ENTRY(MisExpect /* misexpect */, 11955, /* DiagArray550 */ 1565, 0, R"()")
DIAG_ENTRY(MisleadingIndentation /* misleading-indentation */, 11965, /* DiagArray551 */ 1567, 0, R"()")
DIAG_ENTRY(anonymous_265 /* mismatched-new-delete */, 11988, /* DiagArray552 */ 1569, 0, R"()")
DIAG_ENTRY(MismatchedParameterTypes /* mismatched-parameter-types */, 12010, /* DiagArray553 */ 1571, 0, R"()")
DIAG_ENTRY(MismatchedReturnTypes /* mismatched-return-types */, 12037, /* DiagArray554 */ 1573, 0, R"()")
DIAG_ENTRY(MismatchedTags /* mismatched-tags */, 12061, /* DiagArray555 */ 1575, 0, R"()")
DIAG_ENTRY(MissingBraces /* missing-braces */, 12077, /* DiagArray556 */ 1578, 0, R"()")
DIAG_ENTRY(anonymous_282 /* missing-constinit */, 12092, /* DiagArray557 */ 1580, 0, R"()")
DIAG_ENTRY(MissingDeclarations /* missing-declarations */, 12110, /* DiagArray558 */ 1582, 0, R"()")
DIAG_ENTRY(MissingDesignatedFieldInitializers /* missing-designated-field-initializers */, 12131, /* DiagArray559 */ 1587, 0, R"(Warn about designated initializers with some fields missing (only in C++).)")
DIAG_ENTRY(anonymous_192 /* missing-exception-spec */, 12169, /* DiagArray560 */ 1589, 0, R"()")
DIAG_ENTRY(MissingFieldInitializers /* missing-field-initializers */, 12192, /* DiagArray561 */ 1591, /* DiagSubGroup561 */ 430, R"()")
DIAG_ENTRY(anonymous_34 /* missing-format-attribute */, 12219, 0, 0, R"()")
DIAG_ENTRY(anonymous_35 /* missing-include-dirs */, 12244, 0, 0, R"()")
DIAG_ENTRY(MissingMethodReturnType /* missing-method-return-type */, 12265, /* DiagArray564 */ 1593, 0, R"()")
DIAG_ENTRY(anonymous_105 /* missing-multilib */, 12292, /* DiagArray565 */ 1595, 0, R"()")
DIAG_ENTRY(MissingNoEscape /* missing-noescape */, 12309, /* DiagArray566 */ 1597, 0, R"()")
DIAG_ENTRY(MissingNoreturn /* missing-noreturn */, 12326, /* DiagArray567 */ 1599, 0, R"()")
DIAG_ENTRY(anonymous_214 /* missing-prototype-for-cc */, 12343, /* DiagArray568 */ 1602, 0, R"()")
DIAG_ENTRY(anonymous_232 /* missing-prototypes */, 12368, /* DiagArray569 */ 1604, 0, R"()")
DIAG_ENTRY(anonymous_150 /* missing-selector-name */, 12387, /* DiagArray570 */ 1606, 0, R"()")
DIAG_ENTRY(anonymous_96 /* missing-sysroot */, 12409, /* DiagArray571 */ 1608, 0, R"()")
DIAG_ENTRY(anonymous_233 /* missing-variable-declarations */, 12425, /* DiagArray572 */ 1610, 0, R"()")
DIAG_ENTRY(MisspelledAssumption /* misspelled-assumption */, 12455, /* DiagArray573 */ 1612, 0, R"()")
DIAG_ENTRY(HLSLMixPackOffset /* mix-packoffset */, 12477, /* DiagArray574 */ 1614, 0, R"()")
DIAG_ENTRY(ModuleBuild /* module-build */, 12492, /* DiagArray575 */ 1616, 0, R"()")
DIAG_ENTRY(ModuleConflict /* module-conflict */, 12505, /* DiagArray576 */ 1621, 0, R"()")
DIAG_ENTRY(anonymous_110 /* module-file-config-mismatch */, 12521, /* DiagArray577 */ 1624, 0, R"()")
DIAG_ENTRY(ModuleFileExtension /* module-file-extension */, 12549, /* DiagArray578 */ 1626, 0, R"()")
DIAG_ENTRY(ModuleImport /* module-import */, 12571, /* DiagArray579 */ 1628, 0, R"()")
DIAG_ENTRY(anonymous_298 /* module-import-in-extern-c */, 12585, /* DiagArray580 */ 1630, 0, R"()")
DIAG_ENTRY(ModuleIncludeDirectiveTranslation /* module-include-translation */, 12611, /* DiagArray581 */ 1632, 0, R"()")
DIAG_ENTRY(ModuleLock /* module-lock */, 12638, /* DiagArray582 */ 1634, 0, R"()")
DIAG_ENTRY(anonymous_301 /* modules-ambiguous-internal-linkage */, 12650, /* DiagArray583 */ 1636, 0, R"()")
DIAG_ENTRY(anonymous_299 /* modules-import-nested-redundant */, 12685, /* DiagArray584 */ 1638, 0, R"()")
DIAG_ENTRY(Most /* most */, 12717, 0, /* DiagSubGroup585 */ 432, R"()")
DIAG_ENTRY(Move /* move */, 12722, 0, /* DiagSubGroup586 */ 470, R"()")
DIAG_ENTRY(anonymous_76 /* msvc-include */, 12727, 0, /* DiagSubGroup587 */ 475, R"()")
DIAG_ENTRY(anonymous_101 /* msvc-not-found */, 12740, /* DiagArray588 */ 1640, 0, R"()")
DIAG_ENTRY(MultiGPU /* multi-gpu */, 12755, /* DiagArray589 */ 1642, 0, R"()")
DIAG_ENTRY(MultiChar /* multichar */, 12765, /* DiagArray590 */ 1644, 0, R"()")
DIAG_ENTRY(anonymous_287 /* multiple-move-vbase */, 12775, /* DiagArray591 */ 1646, 0, R"()")
DIAG_ENTRY(anonymous_85 /* nan-infinity-disabled */, 12795, /* DiagArray592 */ 1648, 0, R"(This warning is enabled when source code using the macros ``INFINITY`` or ``NAN``
is compiled with floating-point options preventing these two values. This can
lead to undefined behavior. Check the order of command line arguments that modify
this behavior, such as ``-ffast-math``, ``-fhonor-infinities``, and
``-fhonor-nans`` (etc), as well as ``#pragma`` directives if this diagnostic is
generated unexpectedly.)")
DIAG_ENTRY(anonymous_24 /* narrowing */, 12817, 0, /* DiagSubGroup593 */ 477, R"()")
DIAG_ENTRY(anonymous_284 /* nested-anon-types */, 12827, /* DiagArray594 */ 1650, 0, R"()")
DIAG_ENTRY(anonymous_36 /* nested-externs */, 12845, 0, 0, R"()")
DIAG_ENTRY(OperatorNewReturnsNull /* new-returns-null */, 12860, /* DiagArray596 */ 1652, 0, R"()")
DIAG_ENTRY(NewlineEOF /* newline-eof */, 12877, /* DiagArray597 */ 1654, 0, R"()")
DIAG_ENTRY(NoDeref /* noderef */, 12889, /* DiagArray598 */ 1657, 0, R"()")
DIAG_ENTRY(NoexceptType /* noexcept-type */, 12897, 0, /* DiagSubGroup599 */ 479, R"()")
DIAG_ENTRY(anonymous_172 /* non-c-typedef-for-linkage */, 12911, /* DiagArray600 */ 1661, 0, R"()")
DIAG_ENTRY(NonGCC /* non-gcc */, 12937, 0, /* DiagSubGroup601 */ 481, R"()")
DIAG_ENTRY(NonLiteralNullConversion /* non-literal-null-conversion */, 12945, /* DiagArray602 */ 1663, 0, R"()")
DIAG_ENTRY(NonModularIncludeInFrameworkModule /* non-modular-include-in-framework-module */, 12973, /* DiagArray603 */ 1665, 0, R"()")
DIAG_ENTRY(NonModularIncludeInModule /* non-modular-include-in-module */, 13013, /* DiagArray604 */ 1667, /* DiagSubGroup604 */ 485, R"()")
DIAG_ENTRY(NonPODVarargs /* non-pod-varargs */, 13043, /* DiagArray605 */ 1669, 0, R"()")
DIAG_ENTRY(anonymous_205 /* non-power-of-two-alignment */, 13059, /* DiagArray606 */ 1674, 0, R"()")
DIAG_ENTRY(NonVirtualDtor /* non-virtual-dtor */, 13086, /* DiagArray607 */ 1676, 0, R"()")
DIAG_ENTRY(NonNull /* nonnull */, 13103, /* DiagArray608 */ 1678, 0, R"()")
DIAG_ENTRY(anonymous_38 /* nonportable-cfstrings */, 13111, 0, 0, R"()")
DIAG_ENTRY(anonymous_126 /* nonportable-include-path */, 13133, /* DiagArray610 */ 1681, 0, R"()")
DIAG_ENTRY(anonymous_92 /* nonportable-private-apinotes-path */, 13158, /* DiagArray611 */ 1683, 0, R"()")
DIAG_ENTRY(anonymous_93 /* nonportable-private-system-apinotes-path */, 13192, /* DiagArray612 */ 1685, 0, R"()")
DIAG_ENTRY(anonymous_127 /* nonportable-system-include-path */, 13233, /* DiagArray613 */ 1687, 0, R"()")
DIAG_ENTRY(anonymous_293 /* nonportable-vector-initialization */, 13265, /* DiagArray614 */ 1689, 0, R"()")
DIAG_ENTRY(NonTrivialMemaccess /* nontrivial-memaccess */, 13299, /* DiagArray615 */ 1691, 0, R"()")
DIAG_ENTRY(NSConsumedMismatch /* nsconsumed-mismatch */, 13320, /* DiagArray616 */ 1693, 0, R"()")
DIAG_ENTRY(NSReturnsMismatch /* nsreturns-mismatch */, 13340, /* DiagArray617 */ 1695, 0, R"()")
DIAG_ENTRY(NullArithmetic /* null-arithmetic */, 13359, /* DiagArray618 */ 1697, 0, R"()")
DIAG_ENTRY(NullCharacter /* null-character */, 13375, /* DiagArray619 */ 1700, 0, R"()")
DIAG_ENTRY(NullConversion /* null-conversion */, 13390, /* DiagArray620 */ 1703, 0, R"()")
DIAG_ENTRY(NullDereference /* null-dereference */, 13406, /* DiagArray621 */ 1705, 0, R"()")
DIAG_ENTRY(NullPointerArithmetic /* null-pointer-arithmetic */, 13423, /* DiagArray622 */ 1708, /* DiagSubGroup622 */ 487, R"()")
DIAG_ENTRY(NullPointerSubtraction /* null-pointer-subtraction */, 13447, /* DiagArray623 */ 1710, 0, R"()")
DIAG_ENTRY(Nullability /* nullability */, 13472, /* DiagArray624 */ 1712, 0, R"()")
DIAG_ENTRY(NullabilityCompleteness /* nullability-completeness */, 13484, /* DiagArray625 */ 1718, /* DiagSubGroup625 */ 489, R"()")
DIAG_ENTRY(NullabilityCompletenessOnArrays /* nullability-completeness-on-arrays */, 13509, /* DiagArray626 */ 1720, 0, R"()")
DIAG_ENTRY(NullabilityDeclSpec /* nullability-declspec */, 13544, /* DiagArray627 */ 1722, 0, R"()")
DIAG_ENTRY(anonymous_144 /* nullability-extension */, 13565, /* DiagArray628 */ 1724, 0, R"()")
DIAG_ENTRY(NullabilityInferredOnNestedType /* nullability-inferred-on-nested-type */, 13587, /* DiagArray629 */ 1726, 0, R"()")
DIAG_ENTRY(NullableToNonNullConversion /* nullable-to-nonnull-conversion */, 13623, /* DiagArray630 */ 1728, 0, R"()")
DIAG_ENTRY(anonymous_181 /* objc-autosynthesis-property-ivar-name-match */, 13654, /* DiagArray631 */ 1730, 0, R"()")
DIAG_ENTRY(ObjCBoolConstantConversion /* objc-bool-constant-conversion */, 13698, /* DiagArray632 */ 1732, 0, R"()")
DIAG_ENTRY(ObjCBoxing /* objc-boxing */, 13728, /* DiagArray633 */ 1734, 0, R"()")
DIAG_ENTRY(anonymous_262 /* objc-circular-container */, 13740, /* DiagArray634 */ 1736, 0, R"()")
DIAG_ENTRY(ObjCCocoaAPI /* objc-cocoa-api */, 13764, 0, /* DiagSubGroup635 */ 491, R"()")
DIAG_ENTRY(ObjCDesignatedInit /* objc-designated-initializers */, 13779, /* DiagArray636 */ 1738, 0, R"()")
DIAG_ENTRY(anonymous_202 /* objc-dictionary-duplicate-keys */, 13808, /* DiagArray637 */ 1745, 0, R"()")
DIAG_ENTRY(anonymous_88 /* objc-duplicate-category-definition */, 13839, /* DiagArray638 */ 1747, 0, R"()")
DIAG_ENTRY(ObjCFlexibleArray /* objc-flexible-array */, 13874, /* DiagArray639 */ 1749, 0, R"()")
DIAG_ENTRY(anonymous_239 /* objc-forward-class-redefinition */, 13894, /* DiagArray640 */ 1752, 0, R"()")
DIAG_ENTRY(anonymous_240 /* objc-interface-ivars */, 13926, /* DiagArray641 */ 1754, 0, R"()")
DIAG_ENTRY(ObjCLiteralComparison /* objc-literal-compare */, 13947, /* DiagArray642 */ 1756, /* DiagSubGroup642 */ 493, R"()")
DIAG_ENTRY(ObjCLiteralConversion /* objc-literal-conversion */, 13968, /* DiagArray643 */ 1758, 0, R"()")
DIAG_ENTRY(anonymous_134 /* objc-macro-redefinition */, 13992, /* DiagArray644 */ 1761, 0, R"()")
DIAG_ENTRY(anonymous_186 /* objc-messaging-id */, 14016, /* DiagArray645 */ 1763, 0, R"()")
DIAG_ENTRY(MethodAccess /* objc-method-access */, 14034, /* DiagArray646 */ 1765, 0, R"()")
DIAG_ENTRY(anonymous_182 /* objc-missing-property-synthesis */, 14053, /* DiagArray647 */ 1772, 0, R"()")
DIAG_ENTRY(ObjCMissingSuperCalls /* objc-missing-super-calls */, 14085, /* DiagArray648 */ 1774, 0, R"()")
DIAG_ENTRY(ObjCMultipleMethodNames /* objc-multiple-method-names */, 14110, /* DiagArray649 */ 1776, 0, R"()")
DIAG_ENTRY(ObjCRetainBlockProperty /* objc-noncopy-retain-block-property */, 14137, /* DiagArray650 */ 1778, 0, R"()")
DIAG_ENTRY(ObjCNonUnifiedException /* objc-nonunified-exceptions */, 14172, /* DiagArray651 */ 1780, 0, R"()")
DIAG_ENTRY(ObjCPropertyAssignOnObjectType /* objc-property-assign-on-object-type */, 14199, /* DiagArray652 */ 1782, 0, R"()")
DIAG_ENTRY(ObjCPropertyImpl /* objc-property-implementation */, 14235, /* DiagArray653 */ 1784, 0, R"()")
DIAG_ENTRY(anonymous_177 /* objc-property-implicit-mismatch */, 14264, /* DiagArray654 */ 1789, 0, R"()")
DIAG_ENTRY(anonymous_179 /* objc-property-matches-cocoa-ownership-rule */, 14296, /* DiagArray655 */ 1791, 0, R"()")
DIAG_ENTRY(ObjCPropertyNoAttribute /* objc-property-no-attribute */, 14339, /* DiagArray656 */ 1793, 0, R"()")
DIAG_ENTRY(ObjCNoPropertyAutoSynthesis /* objc-property-synthesis */, 14366, /* DiagArray657 */ 1796, 0, R"()")
DIAG_ENTRY(ObjCProtocolMethodImpl /* objc-protocol-method-implementation */, 14390, /* DiagArray658 */ 1800, 0, R"()")
DIAG_ENTRY(anonymous_180 /* objc-protocol-property-synthesis */, 14426, /* DiagArray659 */ 1802, 0, R"()")
DIAG_ENTRY(ObjCProtocolQualifiers /* objc-protocol-qualifiers */, 14459, /* DiagArray660 */ 1804, 0, R"()")
DIAG_ENTRY(ObjCReadonlyPropertyHasSetter /* objc-readonly-with-setter-property */, 14484, /* DiagArray661 */ 1806, 0, R"()")
DIAG_ENTRY(ObjCRedundantAPIUse /* objc-redundant-api-use */, 14519, 0, /* DiagSubGroup662 */ 495, R"()")
DIAG_ENTRY(ObjCRedundantLiteralUse /* objc-redundant-literal-use */, 14542, /* DiagArray663 */ 1808, 0, R"()")
DIAG_ENTRY(ObjCRootClass /* objc-root-class */, 14569, /* DiagArray664 */ 1810, 0, R"()")
DIAG_ENTRY(ObjCSignedCharBool /* objc-signed-char-bool */, 14585, 0, /* DiagSubGroup665 */ 497, R"()")
DIAG_ENTRY(ObjCSignedCharBoolImplicitFloatConversion /* objc-signed-char-bool-implicit-float-conversion */, 14607, /* DiagArray666 */ 1812, 0, R"()")
DIAG_ENTRY(ObjCSignedCharBoolImplicitIntConversion /* objc-signed-char-bool-implicit-int-conversion */, 14655, /* DiagArray667 */ 1814, 0, R"()")
DIAG_ENTRY(ObjCStringComparison /* objc-string-compare */, 14701, /* DiagArray668 */ 1816, 0, R"()")
DIAG_ENTRY(ObjCStringConcatenation /* objc-string-concatenation */, 14721, /* DiagArray669 */ 1818, 0, R"()")
DIAG_ENTRY(anonymous_263 /* objc-unsafe-perform-selector */, 14747, /* DiagArray670 */ 1820, 0, R"()")
DIAG_ENTRY(ODR /* odr */, 14776, /* DiagArray671 */ 1822, 0, R"()")
DIAG_ENTRY(OldStyleCast /* old-style-cast */, 14780, /* DiagArray672 */ 1841, 0, R"()")
DIAG_ENTRY(anonymous_40 /* old-style-definition */, 14795, 0, 0, R"()")
DIAG_ENTRY(OpenACC /* openacc */, 14816, 0, /* DiagSubGroup674 */ 502, R"()")
DIAG_ENTRY(anonymous_305 /* openacc-deprecated-clause-alias */, 14824, /* DiagArray675 */ 1843, 0, R"()")
DIAG_ENTRY(anonymous_304 /* openacc-self-if-potential-conflict */, 14856, /* DiagArray676 */ 1845, 0, R"()")
DIAG_ENTRY(OpenCLUnsupportedRGBA /* opencl-unsupported-rgba */, 14891, /* DiagArray677 */ 1847, 0, R"()")
DIAG_ENTRY(OpenMP /* openmp */, 14915, 0, /* DiagSubGroup678 */ 504, R"()")
DIAG_ENTRY(OpenMP51Ext /* openmp-51-extensions */, 14922, /* DiagArray679 */ 1849, 0, R"()")
DIAG_ENTRY(OpenMPClauses /* openmp-clauses */, 14943, /* DiagArray680 */ 1851, 0, R"()")
DIAG_ENTRY(OpenMPExtensions /* openmp-extensions */, 14958, /* DiagArray681 */ 1871, 0, R"()")
DIAG_ENTRY(OpenMPLoopForm /* openmp-loop-form */, 14976, /* DiagArray682 */ 1873, 0, R"()")
DIAG_ENTRY(OpenMPMapping /* openmp-mapping */, 14993, /* DiagArray683 */ 1876, 0, R"()")
DIAG_ENTRY(OpenMPTarget /* openmp-target */, 15008, /* DiagArray684 */ 1878, /* DiagSubGroup684 */ 513, R"()")
DIAG_ENTRY(OpenMPTargetException /* openmp-target-exception */, 15022, /* DiagArray685 */ 1882, 0, R"()")
DIAG_ENTRY(OptionIgnored /* option-ignored */, 15046, /* DiagArray686 */ 1885, 0, R"()")
DIAG_ENTRY(OrderedCompareFunctionPointers /* ordered-compare-function-pointers */, 15061, /* DiagArray687 */ 1904, 0, R"()")
DIAG_ENTRY(OutOfLineDeclaration /* out-of-line-declaration */, 15095, /* DiagArray688 */ 1907, 0, R"()")
DIAG_ENTRY(anonymous_164 /* out-of-scope-function */, 15119, /* DiagArray689 */ 1909, 0, R"()")
DIAG_ENTRY(OveralignedType /* over-aligned */, 15141, /* DiagArray690 */ 1911, 0, R"()")
DIAG_ENTRY(anonymous_41 /* overflow */, 15154, 0, 0, R"()")
DIAG_ENTRY(OverlengthStrings /* overlength-strings */, 15163, /* DiagArray692 */ 1913, 0, R"()")
DIAG_ENTRY(OverloadedShiftOpParentheses /* overloaded-shift-op-parentheses */, 15182, /* DiagArray693 */ 1915, 0, R"()")
DIAG_ENTRY(OverloadedVirtual /* overloaded-virtual */, 15214, /* DiagArray694 */ 1917, 0, R"()")
DIAG_ENTRY(anonymous_37 /* override-init */, 15233, 0, /* DiagSubGroup695 */ 515, R"()")
DIAG_ENTRY(anonymous_107 /* override-module */, 15247, /* DiagArray696 */ 1919, 0, R"()")
DIAG_ENTRY(OverridingMethodMismatch /* overriding-method-mismatch */, 15263, /* DiagArray697 */ 1921, 0, R"()")
DIAG_ENTRY(anonymous_95 /* overriding-option */, 15290, /* DiagArray698 */ 1929, 0, R"()")
DIAG_ENTRY(Packed /* packed */, 15308, /* DiagArray699 */ 1931, /* DiagSubGroup699 */ 517, R"()")
DIAG_ENTRY(PackedNonPod /* packed-non-pod */, 15315, /* DiagArray700 */ 1933, 0, R"()")
DIAG_ENTRY(Padded /* padded */, 15330, /* DiagArray701 */ 1935, /* DiagSubGroup701 */ 519, R"()")
DIAG_ENTRY(PaddedBitField /* padded-bitfield */, 15337, /* DiagArray702 */ 1939, 0, R"()")
DIAG_ENTRY(Parentheses /* parentheses */, 15353, /* DiagArray703 */ 1942, /* DiagSubGroup703 */ 521, R"()")
DIAG_ENTRY(ParenthesesOnEquality /* parentheses-equality */, 15365, /* DiagArray704 */ 1947, 0, R"()")
DIAG_ENTRY(anonymous_8 /* partial-availability */, 15386, 0, /* DiagSubGroup705 */ 530, R"()")
DIAG_ENTRY(BackendOptimizationRemark /* pass */, 15407, /* DiagArray706 */ 1949, 0, R"()")
DIAG_ENTRY(BackendOptimizationRemarkAnalysis /* pass-analysis */, 15412, /* DiagArray707 */ 1951, 0, R"()")
DIAG_ENTRY(BackendOptimizationFailure /* pass-failed */, 15426, /* DiagArray708 */ 1955, 0, R"()")
DIAG_ENTRY(BackendOptimizationRemarkMissed /* pass-missed */, 15438, /* DiagArray709 */ 1957, 0, R"()")
DIAG_ENTRY(anonymous_307 /* pch-date-time */, 15450, /* DiagArray710 */ 1959, 0, R"()")
DIAG_ENTRY(anonymous_306 /* pch-vfs-diff */, 15464, /* DiagArray711 */ 1961, 0, R"()")
DIAG_ENTRY(Pedantic /* pedantic */, 15477, /* DiagArray712 */ 1963, /* DiagSubGroup712 */ 532, R"()")
DIAG_ENTRY(OpenCLCoreFeaturesDiagGroup /* pedantic-core-features */, 15486, /* DiagArray713 */ 2046, 0, R"()")
DIAG_ENTRY(PedanticMacros /* pedantic-macros */, 15509, 0, /* DiagSubGroup714 */ 591, R"()")
DIAG_ENTRY(PessimizingMove /* pessimizing-move */, 15525, /* DiagArray715 */ 2049, 0, R"()")
DIAG_ENTRY(PointerArith /* pointer-arith */, 15542, /* DiagArray716 */ 2052, /* DiagSubGroup716 */ 597, R"()")
DIAG_ENTRY(PointerBoolConversion /* pointer-bool-conversion */, 15556, /* DiagArray717 */ 2056, 0, R"()")
DIAG_ENTRY(anonymous_217 /* pointer-compare */, 15580, /* DiagArray718 */ 2059, 0, R"()")
DIAG_ENTRY(anonymous_261 /* pointer-integer-compare */, 15596, /* DiagArray719 */ 2061, 0, R"()")
DIAG_ENTRY(anonymous_273 /* pointer-sign */, 15620, /* DiagArray720 */ 2063, 0, R"()")
DIAG_ENTRY(PointerToEnumCast /* pointer-to-enum-cast */, 15633, /* DiagArray721 */ 2065, /* DiagSubGroup721 */ 599, R"()")
DIAG_ENTRY(PointerToIntCast /* pointer-to-int-cast */, 15654, /* DiagArray722 */ 2067, /* DiagSubGroup722 */ 601, R"()")
DIAG_ENTRY(anonymous_279 /* pointer-type-mismatch */, 15674, /* DiagArray723 */ 2069, 0, R"()")
DIAG_ENTRY(anonymous_94 /* poison-system-directories */, 15696, /* DiagArray724 */ 2071, 0, R"()")
DIAG_ENTRY(ObjCPotentiallyDirectSelector /* potentially-direct-selector */, 15722, /* DiagArray725 */ 2073, 0, R"()")
DIAG_ENTRY(PotentiallyEvaluatedExpression /* potentially-evaluated-expression */, 15750, /* DiagArray726 */ 2075, 0, R"()")
DIAG_ENTRY(PragmaClangAttribute /* pragma-clang-attribute */, 15783, /* DiagArray727 */ 2077, 0, R"()")
DIAG_ENTRY(anonymous_128 /* pragma-once-outside-header */, 15806, /* DiagArray728 */ 2079, 0, R"()")
DIAG_ENTRY(PragmaPack /* pragma-pack */, 15833, /* DiagArray729 */ 2081, /* DiagSubGroup729 */ 604, R"()")
DIAG_ENTRY(PragmaPackSuspiciousInclude /* pragma-pack-suspicious-include */, 15845, /* DiagArray730 */ 2084, 0, R"()")
DIAG_ENTRY(anonymous_129 /* pragma-system-header-outside-header */, 15876, /* DiagArray731 */ 2086, 0, R"()")
DIAG_ENTRY(Pragmas /* pragmas */, 15912, /* DiagArray732 */ 2088, /* DiagSubGroup732 */ 606, R"()")
DIAG_ENTRY(CXXPre14Compat /* pre-c++14-compat */, 15920, /* DiagArray733 */ 2091, 0, R"()")
DIAG_ENTRY(CXXPre14CompatPedantic /* pre-c++14-compat-pedantic */, 15937, 0, /* DiagSubGroup734 */ 611, R"()")
DIAG_ENTRY(CXXPre17Compat /* pre-c++17-compat */, 15963, /* DiagArray735 */ 2103, 0, R"()")
DIAG_ENTRY(CXXPre17CompatPedantic /* pre-c++17-compat-pedantic */, 15980, /* DiagArray736 */ 2121, /* DiagSubGroup736 */ 614, R"()")
DIAG_ENTRY(CXXPre20Compat /* pre-c++20-compat */, 16006, /* DiagArray737 */ 2124, 0, R"()")
DIAG_ENTRY(CXXPre20CompatPedantic /* pre-c++20-compat-pedantic */, 16023, /* DiagArray738 */ 2153, /* DiagSubGroup738 */ 616, R"()")
DIAG_ENTRY(CXXPre23Compat /* pre-c++23-compat */, 16049, /* DiagArray739 */ 2156, 0, R"()")
DIAG_ENTRY(CXXPre23CompatPedantic /* pre-c++23-compat-pedantic */, 16066, 0, /* DiagSubGroup740 */ 618, R"()")
DIAG_ENTRY(CXXPre26Compat /* pre-c++26-compat */, 16092, /* DiagArray741 */ 2171, 0, R"()")
DIAG_ENTRY(CXXPre26CompatPedantic /* pre-c++26-compat-pedantic */, 16109, 0, /* DiagSubGroup742 */ 620, R"()")
DIAG_ENTRY(anonymous_21 /* pre-c++2c-compat */, 16135, 0, /* DiagSubGroup743 */ 622, R"()")
DIAG_ENTRY(anonymous_22 /* pre-c++2c-compat-pedantic */, 16152, 0, /* DiagSubGroup744 */ 624, R"()")
DIAG_ENTRY(CPre11Compat /* pre-c11-compat */, 16178, /* DiagArray745 */ 2176, 0, R"()")
DIAG_ENTRY(CPre11CompatPedantic /* pre-c11-compat-pedantic */, 16193, 0, /* DiagSubGroup746 */ 626, R"()")
DIAG_ENTRY(CPre23Compat /* pre-c23-compat */, 16217, /* DiagArray747 */ 2178, 0, R"()")
DIAG_ENTRY(CPre23CompatPedantic /* pre-c23-compat-pedantic */, 16232, 0, /* DiagSubGroup748 */ 628, R"()")
DIAG_ENTRY(anonymous_13 /* pre-c2x-compat */, 16256, 0, /* DiagSubGroup749 */ 630, R"()")
DIAG_ENTRY(anonymous_14 /* pre-c2x-compat-pedantic */, 16271, 0, /* DiagSubGroup750 */ 632, R"()")
DIAG_ENTRY(OpenMPPre51Compat /* pre-openmp-51-compat */, 16295, /* DiagArray751 */ 2194, 0, R"()")
DIAG_ENTRY(anonymous_160 /* predefined-identifier-outside-function */, 16316, /* DiagArray752 */ 2196, 0, R"()")
DIAG_ENTRY(PrivateExtern /* private-extern */, 16355, /* DiagArray753 */ 2198, 0, R"()")
DIAG_ENTRY(anonymous_140 /* private-header */, 16370, /* DiagArray754 */ 2200, 0, R"()")
DIAG_ENTRY(PrivateModule /* private-module */, 16385, /* DiagArray755 */ 2202, 0, R"()")
DIAG_ENTRY(ProfileInstrMissing /* profile-instr-missing */, 16400, /* DiagArray756 */ 2207, 0, R"()")
DIAG_ENTRY(ProfileInstrOutOfDate /* profile-instr-out-of-date */, 16422, /* DiagArray757 */ 2209, 0, R"()")
DIAG_ENTRY(ProfileInstrUnprofiled /* profile-instr-unprofiled */, 16448, /* DiagArray758 */ 2211, 0, R"()")
DIAG_ENTRY(PropertyAccessDotSyntax /* property-access-dot-syntax */, 16473, /* DiagArray759 */ 2213, 0, R"()")
DIAG_ENTRY(PropertyAttr /* property-attribute-mismatch */, 16500, /* DiagArray760 */ 2215, 0, R"()")
DIAG_ENTRY(Protocol /* protocol */, 16528, /* DiagArray761 */ 2220, 0, R"()")
DIAG_ENTRY(anonymous_175 /* protocol-property-synthesis-ambiguity */, 16537, /* DiagArray762 */ 2222, 0, R"()")
DIAG_ENTRY(anonymous_112 /* psabi */, 16575, /* DiagArray763 */ 2224, 0, R"()")
DIAG_ENTRY(PtrAuthNullPointers /* ptrauth-null-pointers */, 16581, /* DiagArray764 */ 2226, 0, R"()")
DIAG_ENTRY(anonymous_245 /* qualified-void-return-type */, 16603, /* DiagArray765 */ 2229, 0, R"()")
DIAG_ENTRY(FrameworkHdrQuotedInclude /* quoted-include-in-framework-header */, 16630, /* DiagArray766 */ 2231, 0, R"()")
DIAG_ENTRY(RangeLoopAnalysis /* range-loop-analysis */, 16665, 0, /* DiagSubGroup767 */ 634, R"()")
DIAG_ENTRY(RangeLoopBindReference /* range-loop-bind-reference */, 16685, /* DiagArray768 */ 2233, 0, R"()")
DIAG_ENTRY(RangeLoopConstruct /* range-loop-construct */, 16711, /* DiagArray769 */ 2235, 0, R"()")
DIAG_ENTRY(ReadOnlyPlacementChecks /* read-only-types */, 16732, /* DiagArray770 */ 2238, 0, R"()")
DIAG_ENTRY(anonymous_185 /* readonly-iboutlet-property */, 16748, /* DiagArray771 */ 2240, 0, R"()")
DIAG_ENTRY(ObjCReceiver /* receiver-expr */, 16775, /* DiagArray772 */ 2242, 0, R"()")
DIAG_ENTRY(ForwardClassReceiver /* receiver-forward-class */, 16789, /* DiagArray773 */ 2244, 0, R"()")
DIAG_ENTRY(RedeclaredClassMember /* redeclared-class-member */, 16812, /* DiagArray774 */ 2247, 0, R"()")
DIAG_ENTRY(anonymous_188 /* redundant-consteval-if */, 16836, /* DiagArray775 */ 2249, 0, R"()")
DIAG_ENTRY(anonymous_42 /* redundant-decls */, 16859, 0, 0, R"()")
DIAG_ENTRY(RedundantMove /* redundant-move */, 16875, /* DiagArray777 */ 2251, 0, R"()")
DIAG_ENTRY(anonymous_163 /* redundant-parens */, 16890, /* DiagArray778 */ 2253, 0, R"()")
DIAG_ENTRY(Register /* register */, 16907, /* DiagArray779 */ 2255, /* DiagSubGroup779 */ 637, R"()")
DIAG_ENTRY(ReinterpretBaseClass /* reinterpret-base-class */, 16916, /* DiagArray780 */ 2257, 0, R"()")
DIAG_ENTRY(RemarkBackendPlugin /* remark-backend-plugin */, 16939, /* DiagArray781 */ 2259, 0, R"()")
DIAG_ENTRY(Reorder /* reorder */, 16961, 0, /* DiagSubGroup782 */ 639, R"()")
DIAG_ENTRY(ReorderCtor /* reorder-ctor */, 16969, /* DiagArray783 */ 2261, 0, R"()")
DIAG_ENTRY(ReorderInitList /* reorder-init-list */, 16982, /* DiagArray784 */ 2264, 0, R"()")
DIAG_ENTRY(anonymous_223 /* requires-super-attribute */, 17000, /* DiagArray785 */ 2266, 0, R"()")
DIAG_ENTRY(ReservedIdAsMacroAlias /* reserved-id-macro */, 17025, 0, /* DiagSubGroup786 */ 642, R"()")
DIAG_ENTRY(ReservedIdentifier /* reserved-identifier */, 17043, /* DiagArray787 */ 2268, /* DiagSubGroup787 */ 644, R"()")
DIAG_ENTRY(ReservedIdAsMacro /* reserved-macro-identifier */, 17063, /* DiagArray788 */ 2270, 0, R"()")
DIAG_ENTRY(ReservedModuleIdentifier /* reserved-module-identifier */, 17089, /* DiagArray789 */ 2272, 0, R"()")
DIAG_ENTRY(ReservedUserDefinedLiteral /* reserved-user-defined-literal */, 17116, /* DiagArray790 */ 2274, /* DiagSubGroup790 */ 648, R"()")
DIAG_ENTRY(RestrictExpansionMacro /* restrict-expansion */, 17146, /* DiagArray791 */ 2277, 0, R"()")
DIAG_ENTRY(anonymous_238 /* retained-language-linkage */, 17165, /* DiagArray792 */ 2279, 0, R"()")
DIAG_ENTRY(anonymous_29 /* return-local-addr */, 17191, 0, /* DiagSubGroup793 */ 650, R"()")
DIAG_ENTRY(ReturnMismatch /* return-mismatch */, 17209, /* DiagArray794 */ 2281, 0, R"()")
DIAG_ENTRY(ReturnStackAddress /* return-stack-address */, 17225, /* DiagArray795 */ 2285, 0, R"()")
DIAG_ENTRY(ReturnStdMove /* return-std-move */, 17246, 0, 0, R"()")
DIAG_ENTRY(ReturnType /* return-type */, 17262, /* DiagArray797 */ 2289, /* DiagSubGroup797 */ 652, R"()")
DIAG_ENTRY(ReturnTypeCLinkage /* return-type-c-linkage */, 17274, /* DiagArray798 */ 2296, 0, R"()")
DIAG_ENTRY(anonymous_225 /* rewrite-not-bool */, 17296, /* DiagArray799 */ 2299, 0, R"()")
DIAG_ENTRY(RoundTripCC1Args /* round-trip-cc1-args */, 17313, /* DiagArray800 */ 2301, 0, R"()")
DIAG_ENTRY(RTTI /* rtti */, 17333, /* DiagArray801 */ 2303, 0, R"()")
DIAG_ENTRY(SanitizeAddressRemarks /* sanitize-address */, 17338, /* DiagArray802 */ 2306, 0, R"()")
DIAG_ENTRY(anonymous_104 /* sarif-format-unstable */, 17355, /* DiagArray803 */ 2309, 0, R"()")
DIAG_ENTRY(UsedSearchPath /* search-path-usage */, 17377, /* DiagArray804 */ 2311, 0, R"()")
DIAG_ENTRY(Section /* section */, 17395, /* DiagArray805 */ 2313, 0, R"()")
DIAG_ENTRY(Selector /* selector */, 17403, /* DiagArray806 */ 2317, /* DiagSubGroup806 */ 655, R"()")
DIAG_ENTRY(SelectorTypeMismatch /* selector-type-mismatch */, 17412, /* DiagArray807 */ 2319, 0, R"()")
DIAG_ENTRY(SelfAssignment /* self-assign */, 17435, /* DiagArray808 */ 2321, /* DiagSubGroup808 */ 657, R"()")
DIAG_ENTRY(SelfAssignmentField /* self-assign-field */, 17447, /* DiagArray809 */ 2323, 0, R"()")
DIAG_ENTRY(SelfAssignmentOverloaded /* self-assign-overloaded */, 17465, /* DiagArray810 */ 2325, 0, R"()")
DIAG_ENTRY(SelfMove /* self-move */, 17488, /* DiagArray811 */ 2327, 0, R"()")
DIAG_ENTRY(SemiBeforeMethodBody /* semicolon-before-method-body */, 17498, /* DiagArray812 */ 2329, 0, R"()")
DIAG_ENTRY(Sentinel /* sentinel */, 17527, /* DiagArray813 */ 2331, 0, R"()")
DIAG_ENTRY(anonymous_45 /* sequence-point */, 17536, 0, /* DiagSubGroup814 */ 660, R"()")
DIAG_ENTRY(SerializedDiagnostics /* serialized-diagnostics */, 17551, /* DiagArray815 */ 2334, 0, R"()")
DIAG_ENTRY(Shadow /* shadow */, 17574, /* DiagArray816 */ 2338, /* DiagSubGroup816 */ 662, R"()")
DIAG_ENTRY(ShadowAll /* shadow-all */, 17581, 0, /* DiagSubGroup817 */ 665, R"()")
DIAG_ENTRY(ShadowField /* shadow-field */, 17592, /* DiagArray818 */ 2340, 0, R"()")
DIAG_ENTRY(ShadowFieldInConstructor /* shadow-field-in-constructor */, 17605, /* DiagArray819 */ 2342, /* DiagSubGroup819 */ 670, R"()")
DIAG_ENTRY(ShadowFieldInConstructorModified /* shadow-field-in-constructor-modified */, 17633, /* DiagArray820 */ 2344, 0, R"()")
DIAG_ENTRY(ShadowIvar /* shadow-ivar */, 17670, /* DiagArray821 */ 2346, 0, R"()")
DIAG_ENTRY(ShadowUncapturedLocal /* shadow-uncaptured-local */, 17682, /* DiagArray822 */ 2348, 0, R"()")
DIAG_ENTRY(anonymous_250 /* shift-count-negative */, 17706, /* DiagArray823 */ 2350, 0, R"()")
DIAG_ENTRY(anonymous_251 /* shift-count-overflow */, 17727, /* DiagArray824 */ 2352, 0, R"()")
DIAG_ENTRY(anonymous_249 /* shift-negative-value */, 17748, /* DiagArray825 */ 2354, 0, R"()")
DIAG_ENTRY(ShiftOpParentheses /* shift-op-parentheses */, 17769, /* DiagArray826 */ 2356, 0, R"()")
DIAG_ENTRY(anonymous_252 /* shift-overflow */, 17790, /* DiagArray827 */ 2358, 0, R"()")
DIAG_ENTRY(anonymous_253 /* shift-sign-overflow */, 17805, /* DiagArray828 */ 2360, 0, R"()")
DIAG_ENTRY(Shorten64To32 /* shorten-64-to-32 */, 17825, /* DiagArray829 */ 2362, 0, R"()")
DIAG_ENTRY(SignCompare /* sign-compare */, 17842, /* DiagArray830 */ 2364, 0, R"()")
DIAG_ENTRY(SignConversion /* sign-conversion */, 17855, /* DiagArray831 */ 2366, 0, R"()")
DIAG_ENTRY(anonymous_43 /* sign-promo */, 17871, 0, 0, R"()")
DIAG_ENTRY(SignedEnumBitfield /* signed-enum-bitfield */, 17882, /* DiagArray833 */ 2370, 0, R"()")
DIAG_ENTRY(anonymous_295 /* signed-unsigned-wchar */, 17903, /* DiagArray834 */ 2372, 0, R"()")
DIAG_ENTRY(SingleBitBitFieldConstantConversion /* single-bit-bitfield-constant-conversion */, 17925, /* DiagArray835 */ 2374, 0, R"()")
DIAG_ENTRY(SizeofArrayArgument /* sizeof-array-argument */, 17965, /* DiagArray836 */ 2376, 0, R"()")
DIAG_ENTRY(SizeofArrayDecay /* sizeof-array-decay */, 17987, /* DiagArray837 */ 2378, 0, R"()")
DIAG_ENTRY(anonymous_219 /* sizeof-array-div */, 18006, /* DiagArray838 */ 2380, 0, R"()")
DIAG_ENTRY(anonymous_218 /* sizeof-pointer-div */, 18023, /* DiagArray839 */ 2382, 0, R"()")
DIAG_ENTRY(SizeofPointerMemaccess /* sizeof-pointer-memaccess */, 18042, /* DiagArray840 */ 2384, 0, R"()")
DIAG_ENTRY(anonymous_100 /* slash-u-filename */, 18067, /* DiagArray841 */ 2387, 0, R"()")
DIAG_ENTRY(anonymous_87 /* slh-asm-goto */, 18084, /* DiagArray842 */ 2389, 0, R"()")
DIAG_ENTRY(anonymous_90 /* sloc-usage */, 18097, /* DiagArray843 */ 2391, 0, R"()")
DIAG_ENTRY(UninitializedSometimes /* sometimes-uninitialized */, 18108, /* DiagArray844 */ 2393, 0, R"()")
DIAG_ENTRY(BackendSourceMgr /* source-mgr */, 18132, /* DiagArray845 */ 2395, 0, R"()")
DIAG_ENTRY(SourceUsesOpenACC /* source-uses-openacc */, 18143, /* DiagArray846 */ 2397, 0, R"()")
DIAG_ENTRY(SourceUsesOpenMP /* source-uses-openmp */, 18163, /* DiagArray847 */ 2401, 0, R"()")
DIAG_ENTRY(SpirCompat /* spir-compat */, 18182, /* DiagArray848 */ 2410, 0, R"()")
DIAG_ENTRY(anonymous_78 /* spirv-compat */, 18194, 0, /* DiagSubGroup849 */ 672, R"()")
DIAG_ENTRY(anonymous_83 /* stack-exhausted */, 18207, /* DiagArray850 */ 2412, 0, R"()")
DIAG_ENTRY(anonymous_86 /* stack-protector */, 18223, /* DiagArray851 */ 2414, 0, R"()")
DIAG_ENTRY(StaticFloatInit /* static-float-init */, 18239, /* DiagArray852 */ 2416, /* DiagSubGroup852 */ 674, R"()")
DIAG_ENTRY(StaticInInline /* static-in-inline */, 18257, /* DiagArray853 */ 2418, 0, R"()")
DIAG_ENTRY(anonymous_157 /* static-inline-explicit-instantiation */, 18274, /* DiagArray854 */ 2421, 0, R"()")
DIAG_ENTRY(StaticLocalInInline /* static-local-in-inline */, 18311, /* DiagArray855 */ 2423, 0, R"()")
DIAG_ENTRY(UninitializedStaticSelfInit /* static-self-init */, 18334, /* DiagArray856 */ 2425, 0, R"()")
DIAG_ENTRY(anonymous_103 /* stdlibcxx-not-found */, 18351, /* DiagArray857 */ 2427, 0, R"()")
DIAG_ENTRY(anonymous_49 /* strict-aliasing */, 18371, 0, 0, R"()")
DIAG_ENTRY(anonymous_46 /* strict-aliasing=0 */, 18387, 0, 0, R"()")
DIAG_ENTRY(anonymous_47 /* strict-aliasing=1 */, 18405, 0, 0, R"()")
DIAG_ENTRY(anonymous_48 /* strict-aliasing=2 */, 18423, 0, 0, R"()")
DIAG_ENTRY(anonymous_56 /* strict-overflow */, 18441, 0, 0, R"()")
DIAG_ENTRY(anonymous_50 /* strict-overflow=0 */, 18457, 0, 0, R"()")
DIAG_ENTRY(anonymous_51 /* strict-overflow=1 */, 18475, 0, 0, R"()")
DIAG_ENTRY(anonymous_52 /* strict-overflow=2 */, 18493, 0, 0, R"()")
DIAG_ENTRY(anonymous_53 /* strict-overflow=3 */, 18511, 0, 0, R"()")
DIAG_ENTRY(anonymous_54 /* strict-overflow=4 */, 18529, 0, 0, R"()")
DIAG_ENTRY(anonymous_55 /* strict-overflow=5 */, 18547, 0, 0, R"()")
DIAG_ENTRY(ObjCStrictPotentiallyDirectSelector /* strict-potentially-direct-selector */, 18565, /* DiagArray869 */ 2429, /* DiagSubGroup869 */ 676, R"()")
DIAG_ENTRY(anonymous_226 /* strict-primary-template-shadow */, 18600, /* DiagArray870 */ 2431, 0, R"()")
DIAG_ENTRY(StrictPrototypes /* strict-prototypes */, 18631, /* DiagArray871 */ 2433, /* DiagSubGroup871 */ 678, R"()")
DIAG_ENTRY(StrictSelector /* strict-selector-match */, 18649, /* DiagArray872 */ 2435, 0, R"()")
DIAG_ENTRY(StringCompare /* string-compare */, 18671, /* DiagArray873 */ 2437, 0, R"()")
DIAG_ENTRY(StringConcatation /* string-concatenation */, 18686, /* DiagArray874 */ 2439, 0, R"()")
DIAG_ENTRY(StringConversion /* string-conversion */, 18707, /* DiagArray875 */ 2441, 0, R"()")
DIAG_ENTRY(StringPlusChar /* string-plus-char */, 18725, /* DiagArray876 */ 2443, 0, R"()")
DIAG_ENTRY(StringPlusInt /* string-plus-int */, 18742, /* DiagArray877 */ 2445, 0, R"()")
DIAG_ENTRY(anonymous_168 /* strlcpy-strlcat-size */, 18758, /* DiagArray878 */ 2447, 0, R"()")
DIAG_ENTRY(StrncatSize /* strncat-size */, 18779, /* DiagArray879 */ 2449, 0, R"()")
DIAG_ENTRY(CXX11WarnSuggestOverrideDestructor /* suggest-destructor-override */, 18792, /* DiagArray880 */ 2453, 0, R"()")
DIAG_ENTRY(CXX11WarnSuggestOverride /* suggest-override */, 18820, /* DiagArray881 */ 2455, 0, R"()")
DIAG_ENTRY(SuperSubClassMismatch /* super-class-method-mismatch */, 18837, /* DiagArray882 */ 2457, 0, R"()")
DIAG_ENTRY(SuspiciousBzero /* suspicious-bzero */, 18865, /* DiagArray883 */ 2459, 0, R"()")
DIAG_ENTRY(SuspiciousMemaccess /* suspicious-memaccess */, 18882, 0, /* DiagSubGroup884 */ 680, R"()")
DIAG_ENTRY(SwiftNameAttribute /* swift-name-attribute */, 18903, /* DiagArray885 */ 2461, 0, R"()")
DIAG_ENTRY(Switch /* switch */, 18924, /* DiagArray886 */ 2475, 0, R"()")
DIAG_ENTRY(SwitchBool /* switch-bool */, 18931, /* DiagArray887 */ 2479, 0, R"()")
DIAG_ENTRY(SwitchDefault /* switch-default */, 18943, /* DiagArray888 */ 2481, 0, R"()")
DIAG_ENTRY(SwitchEnum /* switch-enum */, 18958, /* DiagArray889 */ 2483, 0, R"()")
DIAG_ENTRY(SyncAlignment /* sync-alignment */, 18970, /* DiagArray890 */ 2485, 0, R"()")
DIAG_ENTRY(anonymous_294 /* sync-fetch-and-nand-semantics-changed */, 18985, /* DiagArray891 */ 2487, 0, R"()")
DIAG_ENTRY(anonymous_44 /* synth */, 19023, 0, 0, R"()")
DIAG_ENTRY(TargetClonesMixedSpecifiers /* target-clones-mixed-specifiers */, 19029, /* DiagArray893 */ 2489, 0, R"()")
DIAG_ENTRY(TautologicalBitwiseCompare /* tautological-bitwise-compare */, 19060, /* DiagArray894 */ 2491, 0, R"()")
DIAG_ENTRY(TautologicalCompare /* tautological-compare */, 19089, /* DiagArray895 */ 2494, /* DiagSubGroup895 */ 686, R"()")
DIAG_ENTRY(TautologicalConstantCompare /* tautological-constant-compare */, 19110, /* DiagArray896 */ 2497, /* DiagSubGroup896 */ 694, R"()")
DIAG_ENTRY(TautologicalInRangeCompare /* tautological-constant-in-range-compare */, 19140, 0, /* DiagSubGroup897 */ 696, R"()")
DIAG_ENTRY(TautologicalOutOfRangeCompare /* tautological-constant-out-of-range-compare */, 19179, /* DiagArray898 */ 2501, 0, R"()")
DIAG_ENTRY(TautologicalNegationCompare /* tautological-negation-compare */, 19222, /* DiagArray899 */ 2503, 0, R"()")
DIAG_ENTRY(TautologicalObjCBoolCompare /* tautological-objc-bool-compare */, 19252, /* DiagArray900 */ 2506, 0, R"()")
DIAG_ENTRY(TautologicalOverlapCompare /* tautological-overlap-compare */, 19283, /* DiagArray901 */ 2508, 0, R"()")
DIAG_ENTRY(TautologicalPointerCompare /* tautological-pointer-compare */, 19312, /* DiagArray902 */ 2510, 0, R"()")
DIAG_ENTRY(TautologicalTypeLimitCompare /* tautological-type-limit-compare */, 19341, /* DiagArray903 */ 2513, 0, R"()")
DIAG_ENTRY(TautologicalUndefinedCompare /* tautological-undefined-compare */, 19373, /* DiagArray904 */ 2515, 0, R"()")
DIAG_ENTRY(TautologicalUnsignedCharZeroCompare /* tautological-unsigned-char-zero-compare */, 19404, /* DiagArray905 */ 2518, 0, R"()")
DIAG_ENTRY(TautologicalUnsignedEnumZeroCompare /* tautological-unsigned-enum-zero-compare */, 19444, /* DiagArray906 */ 2520, 0, R"()")
DIAG_ENTRY(TautologicalUnsignedZeroCompare /* tautological-unsigned-zero-compare */, 19484, /* DiagArray907 */ 2522, 0, R"()")
DIAG_ENTRY(TautologicalValueRangeCompare /* tautological-value-range-compare */, 19519, /* DiagArray908 */ 2524, 0, R"()")
DIAG_ENTRY(anonymous_303 /* tcb-enforcement */, 19552, /* DiagArray909 */ 2526, 0, R"()")
DIAG_ENTRY(anonymous_270 /* template-in-declaration-name */, 19568, /* DiagArray910 */ 2528, 0, R"()")
DIAG_ENTRY(anonymous_257 /* tentative-definition-array */, 19597, /* DiagArray911 */ 2530, 0, R"()")
DIAG_ENTRY(anonymous_256 /* tentative-definition-incomplete-type */, 19624, /* DiagArray912 */ 2532, 0, R"()")
DIAG_ENTRY(ThreadSafety /* thread-safety */, 19661, 0, /* DiagSubGroup913 */ 699, R"()")
DIAG_ENTRY(ThreadSafetyAnalysis /* thread-safety-analysis */, 19675, /* DiagArray914 */ 2534, 0, R"()")
DIAG_ENTRY(ThreadSafetyAttributes /* thread-safety-attributes */, 19698, /* DiagArray915 */ 2553, 0, R"()")
DIAG_ENTRY(ThreadSafetyBeta /* thread-safety-beta */, 19723, /* DiagArray916 */ 2560, 0, R"()")
DIAG_ENTRY(ThreadSafetyNegative /* thread-safety-negative */, 19742, /* DiagArray917 */ 2562, 0, R"()")
DIAG_ENTRY(ThreadSafetyPrecise /* thread-safety-precise */, 19765, /* DiagArray918 */ 2564, 0, R"()")
DIAG_ENTRY(ThreadSafetyReference /* thread-safety-reference */, 19787, /* DiagArray919 */ 2568, /* DiagSubGroup919 */ 704, R"()")
DIAG_ENTRY(ThreadSafetyReferenceReturn /* thread-safety-reference-return */, 19811, /* DiagArray920 */ 2571, 0, R"()")
DIAG_ENTRY(ThreadSafetyVerbose /* thread-safety-verbose */, 19842, /* DiagArray921 */ 2574, 0, R"()")
DIAG_ENTRY(Trigraphs /* trigraphs */, 19864, /* DiagArray922 */ 2576, 0, R"()")
DIAG_ENTRY(TypeLimits /* type-limits */, 19874, 0, /* DiagSubGroup923 */ 706, R"()")
DIAG_ENTRY(TypeSafety /* type-safety */, 19886, /* DiagArray924 */ 2581, 0, R"()")
DIAG_ENTRY(anonymous_237 /* typedef-redefinition */, 19898, /* DiagArray925 */ 2585, 0, R"()")
DIAG_ENTRY(anonymous_231 /* typename-missing */, 19919, /* DiagArray926 */ 2587, 0, R"()")
DIAG_ENTRY(anonymous_108 /* unable-to-open-stats-file */, 19936, /* DiagArray927 */ 2589, 0, R"()")
DIAG_ENTRY(UnalignedAccess /* unaligned-access */, 19962, /* DiagArray928 */ 2591, 0, R"()")
DIAG_ENTRY(anonymous_297 /* unaligned-qualifier-implicit-cast */, 19979, /* DiagArray929 */ 2593, 0, R"()")
DIAG_ENTRY(UnavailableDeclarations /* unavailable-declarations */, 20013, /* DiagArray930 */ 2595, 0, R"()")
DIAG_ENTRY(UndeclaredSelector /* undeclared-selector */, 20038, /* DiagArray931 */ 2597, 0, R"()")
DIAG_ENTRY(anonymous_132 /* undef */, 20058, /* DiagArray932 */ 2600, 0, R"()")
DIAG_ENTRY(anonymous_133 /* undef-prefix */, 20064, /* DiagArray933 */ 2602, 0, R"()")
DIAG_ENTRY(anonymous_198 /* undefined-arm-streaming */, 20077, /* DiagArray934 */ 2604, 0, R"()")
DIAG_ENTRY(anonymous_199 /* undefined-arm-za */, 20101, /* DiagArray935 */ 2606, 0, R"()")
DIAG_ENTRY(anonymous_200 /* undefined-arm-zt0 */, 20118, /* DiagArray936 */ 2608, 0, R"()")
DIAG_ENTRY(UndefinedBoolConversion /* undefined-bool-conversion */, 20136, /* DiagArray937 */ 2610, 0, R"()")
DIAG_ENTRY(UndefinedFuncTemplate /* undefined-func-template */, 20162, /* DiagArray938 */ 2613, 0, R"()")
DIAG_ENTRY(anonymous_236 /* undefined-inline */, 20186, /* DiagArray939 */ 2615, 0, R"()")
DIAG_ENTRY(anonymous_234 /* undefined-internal */, 20203, /* DiagArray940 */ 2617, 0, R"()")
DIAG_ENTRY(anonymous_235 /* undefined-internal-type */, 20222, /* DiagArray941 */ 2619, 0, R"()")
DIAG_ENTRY(UndefinedReinterpretCast /* undefined-reinterpret-cast */, 20246, /* DiagArray942 */ 2621, 0, R"()")
DIAG_ENTRY(UndefinedVarTemplate /* undefined-var-template */, 20273, /* DiagArray943 */ 2624, 0, R"()")
DIAG_ENTRY(UnderalignedExceptionObject /* underaligned-exception-object */, 20296, /* DiagArray944 */ 2626, 0, R"()")
DIAG_ENTRY(UnevaluatedExpression /* unevaluated-expression */, 20326, /* DiagArray945 */ 2628, /* DiagSubGroup945 */ 711, R"()")
DIAG_ENTRY(UnguardedAvailability /* unguarded-availability */, 20349, /* DiagArray946 */ 2630, /* DiagSubGroup946 */ 713, R"()")
DIAG_ENTRY(UnguardedAvailabilityNew /* unguarded-availability-new */, 20372, /* DiagArray947 */ 2633, 0, R"()")
DIAG_ENTRY(Unicode /* unicode */, 20399, /* DiagArray948 */ 2636, 0, R"()")
DIAG_ENTRY(anonymous_118 /* unicode-homoglyph */, 20407, /* DiagArray949 */ 2644, 0, R"()")
DIAG_ENTRY(anonymous_117 /* unicode-whitespace */, 20425, /* DiagArray950 */ 2646, 0, R"()")
DIAG_ENTRY(anonymous_119 /* unicode-zero-width */, 20444, /* DiagArray951 */ 2648, 0, R"()")
DIAG_ENTRY(Uninitialized /* uninitialized */, 20463, /* DiagArray952 */ 2650, /* DiagSubGroup952 */ 715, R"()")
DIAG_ENTRY(UninitializedConstReference /* uninitialized-const-reference */, 20477, /* DiagArray953 */ 2658, 0, R"()")
DIAG_ENTRY(UnknownArgument /* unknown-argument */, 20507, /* DiagArray954 */ 2660, 0, R"()")
DIAG_ENTRY(UnknownAssumption /* unknown-assumption */, 20524, /* DiagArray955 */ 2664, 0, R"()")
DIAG_ENTRY(UnknownAttributes /* unknown-attributes */, 20543, /* DiagArray956 */ 2666, 0, R"()")
DIAG_ENTRY(CudaUnknownVersion /* unknown-cuda-version */, 20562, /* DiagArray957 */ 2668, 0, R"()")
DIAG_ENTRY(anonymous_137 /* unknown-directives */, 20583, /* DiagArray958 */ 2671, 0, R"()")
DIAG_ENTRY(anonymous_122 /* unknown-escape-sequence */, 20602, /* DiagArray959 */ 2673, 0, R"()")
DIAG_ENTRY(UnknownPragmas /* unknown-pragmas */, 20626, /* DiagArray960 */ 2675, 0, R"()")
DIAG_ENTRY(UnknownSanitizers /* unknown-sanitizers */, 20642, /* DiagArray961 */ 2696, 0, R"()")
DIAG_ENTRY(UnknownWarningOption /* unknown-warning-option */, 20661, /* DiagArray962 */ 2698, 0, R"()")
DIAG_ENTRY(UnnamedTypeTemplateArgs /* unnamed-type-template-args */, 20684, /* DiagArray963 */ 2702, /* DiagSubGroup963 */ 719, R"()")
DIAG_ENTRY(UnneededInternalDecl /* unneeded-internal-declaration */, 20711, /* DiagArray964 */ 2704, 0, R"()")
DIAG_ENTRY(UnneededMemberFunction /* unneeded-member-function */, 20741, /* DiagArray965 */ 2707, 0, R"()")
DIAG_ENTRY(anonymous_227 /* unqualified-std-cast-call */, 20766, /* DiagArray966 */ 2709, 0, R"()")
DIAG_ENTRY(UnreachableCode /* unreachable-code */, 20792, /* DiagArray967 */ 2711, /* DiagSubGroup967 */ 721, R"()")
DIAG_ENTRY(UnreachableCodeAggressive /* unreachable-code-aggressive */, 20809, 0, /* DiagSubGroup968 */ 725, R"()")
DIAG_ENTRY(UnreachableCodeBreak /* unreachable-code-break */, 20837, /* DiagArray969 */ 2713, 0, R"()")
DIAG_ENTRY(UnreachableCodeFallthrough /* unreachable-code-fallthrough */, 20860, /* DiagArray970 */ 2715, 0, R"()")
DIAG_ENTRY(UnreachableCodeGenericAssoc /* unreachable-code-generic-assoc */, 20889, /* DiagArray971 */ 2717, 0, R"()")
DIAG_ENTRY(UnreachableCodeLoopIncrement /* unreachable-code-loop-increment */, 20920, /* DiagArray972 */ 2719, 0, R"()")
DIAG_ENTRY(UnreachableCodeReturn /* unreachable-code-return */, 20952, /* DiagArray973 */ 2721, 0, R"()")
DIAG_ENTRY(UnsafeBufferUsage /* unsafe-buffer-usage */, 20976, /* DiagArray974 */ 2723, /* DiagSubGroup974 */ 729, R"()")
DIAG_ENTRY(UnsafeBufferUsageInContainer /* unsafe-buffer-usage-in-container */, 20996, /* DiagArray975 */ 2726, 0, R"()")
DIAG_ENTRY(Unsequenced /* unsequenced */, 21029, /* DiagArray976 */ 2728, 0, R"()")
DIAG_ENTRY(UnsupportedABI /* unsupported-abi */, 21041, /* DiagArray977 */ 2731, 0, R"()")
DIAG_ENTRY(UnsupportedAbs /* unsupported-abs */, 21057, /* DiagArray978 */ 2734, 0, R"()")
DIAG_ENTRY(anonymous_215 /* unsupported-availability-guard */, 21073, /* DiagArray979 */ 2737, 0, R"()")
DIAG_ENTRY(UnsupportedCB /* unsupported-cb */, 21104, /* DiagArray980 */ 2739, 0, R"()")
DIAG_ENTRY(anonymous_211 /* unsupported-dll-base-class-template */, 21119, /* DiagArray981 */ 2741, 0, R"()")
DIAG_ENTRY(UnsupportedFPOpt /* unsupported-floating-point-opt */, 21155, /* DiagArray982 */ 2743, 0, R"()")
DIAG_ENTRY(UnsupportedFriend /* unsupported-friend */, 21186, /* DiagArray983 */ 2746, 0, R"()")
DIAG_ENTRY(UnsupportedGPOpt /* unsupported-gpopt */, 21205, /* DiagArray984 */ 2749, 0, R"()")
DIAG_ENTRY(UnsupportedNan /* unsupported-nan */, 21223, /* DiagArray985 */ 2751, 0, R"()")
DIAG_ENTRY(UnsupportedTargetOpt /* unsupported-target-opt */, 21239, /* DiagArray986 */ 2754, 0, R"()")
DIAG_ENTRY(anonymous_221 /* unsupported-visibility */, 21262, /* DiagArray987 */ 2757, 0, R"()")
DIAG_ENTRY(anonymous_229 /* unusable-partial-specialization */, 21285, /* DiagArray988 */ 2759, 0, R"()")
DIAG_ENTRY(Unused /* unused */, 21317, 0, /* DiagSubGroup989 */ 731, R"()")
DIAG_ENTRY(UnusedArgument /* unused-argument */, 21324, 0, 0, R"()")
DIAG_ENTRY(UnusedButSetParameter /* unused-but-set-parameter */, 21340, /* DiagArray991 */ 2761, 0, R"()")
DIAG_ENTRY(UnusedButSetVariable /* unused-but-set-variable */, 21365, /* DiagArray992 */ 2763, 0, R"()")
DIAG_ENTRY(UnusedCommandLineArgument /* unused-command-line-argument */, 21389, /* DiagArray993 */ 2765, 0, R"()")
DIAG_ENTRY(UnusedComparison /* unused-comparison */, 21418, /* DiagArray994 */ 2779, 0, R"()")
DIAG_ENTRY(UnusedConstVariable /* unused-const-variable */, 21436, /* DiagArray995 */ 2781, 0, R"()")
DIAG_ENTRY(UnusedExceptionParameter /* unused-exception-parameter */, 21458, /* DiagArray996 */ 2783, 0, R"()")
DIAG_ENTRY(UnusedFunction /* unused-function */, 21485, /* DiagArray997 */ 2785, /* DiagSubGroup997 */ 742, R"()")
DIAG_ENTRY(UnusedGetterReturnValue /* unused-getter-return-value */, 21501, /* DiagArray998 */ 2787, 0, R"()")
DIAG_ENTRY(UnusedLabel /* unused-label */, 21528, /* DiagArray999 */ 2789, 0, R"()")
DIAG_ENTRY(UnusedLambdaCapture /* unused-lambda-capture */, 21541, /* DiagArray1000 */ 2791, 0, R"()")
DIAG_ENTRY(UnusedLocalTypedef /* unused-local-typedef */, 21563, /* DiagArray1001 */ 2793, 0, R"()")
DIAG_ENTRY(anonymous_68 /* unused-local-typedefs */, 21584, 0, /* DiagSubGroup1002 */ 744, R"()")
DIAG_ENTRY(anonymous_131 /* unused-macros */, 21606, /* DiagArray1003 */ 2795, 0, R"()")
DIAG_ENTRY(UnusedMemberFunction /* unused-member-function */, 21620, /* DiagArray1004 */ 2797, /* DiagSubGroup1004 */ 746, R"()")
DIAG_ENTRY(UnusedParameter /* unused-parameter */, 21643, /* DiagArray1005 */ 2799, 0, R"()")
DIAG_ENTRY(UnusedPrivateField /* unused-private-field */, 21660, /* DiagArray1006 */ 2801, 0, R"()")
DIAG_ENTRY(UnusedPropertyIvar /* unused-property-ivar */, 21681, /* DiagArray1007 */ 2803, 0, R"()")
DIAG_ENTRY(UnusedResult /* unused-result */, 21702, /* DiagArray1008 */ 2805, 0, R"()")
DIAG_ENTRY(UnusedTemplate /* unused-template */, 21716, /* DiagArray1009 */ 2808, /* DiagSubGroup1009 */ 748, R"()")
DIAG_ENTRY(UnusedValue /* unused-value */, 21732, /* DiagArray1010 */ 2810, /* DiagSubGroup1010 */ 750, R"()")
DIAG_ENTRY(UnusedVariable /* unused-variable */, 21745, /* DiagArray1011 */ 2818, /* DiagSubGroup1011 */ 754, R"()")
DIAG_ENTRY(anonymous_281 /* unused-volatile-lvalue */, 21761, /* DiagArray1012 */ 2820, 0, R"()")
DIAG_ENTRY(UsedButMarkedUnused /* used-but-marked-unused */, 21784, /* DiagArray1013 */ 2822, 0, R"()")
DIAG_ENTRY(UserDefinedLiterals /* user-defined-literals */, 21807, /* DiagArray1014 */ 2824, 0, R"()")
DIAG_ENTRY(UserDefinedWarnings /* user-defined-warnings */, 21829, /* DiagArray1015 */ 2826, 0, R"()")
DIAG_ENTRY(Varargs /* varargs */, 21851, /* DiagArray1016 */ 2828, 0, R"()")
DIAG_ENTRY(VariadicMacros /* variadic-macros */, 21859, /* DiagArray1017 */ 2832, 0, R"()")
DIAG_ENTRY(anonymous_201 /* vec-elem-size */, 21875, /* DiagArray1018 */ 2836, 0, R"()")
DIAG_ENTRY(VectorConversion /* vector-conversion */, 21889, /* DiagArray1019 */ 2838, 0, R"()")
DIAG_ENTRY(anonymous_67 /* vector-conversions */, 21907, 0, /* DiagSubGroup1020 */ 756, R"()")
DIAG_ENTRY(VexingParse /* vexing-parse */, 21926, /* DiagArray1021 */ 2840, 0, R"()")
DIAG_ENTRY(Visibility /* visibility */, 21939, /* DiagArray1022 */ 2844, 0, R"()")
DIAG_ENTRY(VLA /* vla */, 21950, /* DiagArray1023 */ 2847, /* DiagSubGroup1023 */ 758, R"()")
DIAG_ENTRY(VLACxxExtension /* vla-cxx-extension */, 21954, /* DiagArray1024 */ 2849, /* DiagSubGroup1024 */ 760, R"()")
DIAG_ENTRY(VLAExtension /* vla-extension */, 21972, /* DiagArray1025 */ 2852, /* DiagSubGroup1025 */ 762, R"()")
DIAG_ENTRY(VLAUseStaticAssert /* vla-extension-static-assert */, 21986, /* DiagArray1026 */ 2854, 0, R"()")
DIAG_ENTRY(VoidPointerToEnumCast /* void-pointer-to-enum-cast */, 22014, /* DiagArray1027 */ 2857, 0, R"()")
DIAG_ENTRY(VoidPointerToIntCast /* void-pointer-to-int-cast */, 22040, /* DiagArray1028 */ 2859, /* DiagSubGroup1028 */ 764, R"()")
DIAG_ENTRY(VoidPointerDeref /* void-ptr-dereference */, 22065, /* DiagArray1029 */ 2861, 0, R"()")
DIAG_ENTRY(VolatileRegisterVar /* volatile-register-var */, 22086, 0, 0, R"()")
DIAG_ENTRY(WebAssemblyExceptionSpec /* wasm-exception-spec */, 22108, /* DiagArray1031 */ 2863, 0, R"()")
DIAG_ENTRY(anonymous_191 /* weak-template-vtables */, 22128, /* DiagArray1032 */ 2865, 0, R"()")
DIAG_ENTRY(anonymous_190 /* weak-vtables */, 22150, /* DiagArray1033 */ 2867, 0, R"()")
DIAG_ENTRY(WritableStrings /* writable-strings */, 22163, /* DiagArray1034 */ 2869, /* DiagSubGroup1034 */ 766, R"()")
DIAG_ENTRY(GCCWriteStrings /* write-strings */, 22180, 0, /* DiagSubGroup1035 */ 768, R"(**Note:** enabling this warning in C will change the semantic behavior of the
program by treating all string literals as having type ``const char *``
instead of ``char *``. This can cause unexpected behaviors with type-sensitive
constructs like ``_Generic``.)")
DIAG_ENTRY(XorUsedAsPow /* xor-used-as-pow */, 22194, /* DiagArray1036 */ 2871, 0, R"()")
DIAG_ENTRY(anonymous_302 /* zero-as-null-pointer-constant */, 22210, /* DiagArray1037 */ 2875, 0, R"()")
DIAG_ENTRY(ZeroLengthArray /* zero-length-array */, 22240, /* DiagArray1038 */ 2877, 0, R"()")
#endif // DIAG_ENTRY


#ifdef GET_CATEGORY_TABLE
CATEGORY("", DiagCat_None)
CATEGORY("Lexical or Preprocessor Issue", DiagCat_Lexical_or_Preprocessor_Issue)
CATEGORY("Semantic Issue", DiagCat_Semantic_Issue)
CATEGORY("Parse Issue", DiagCat_Parse_Issue)
CATEGORY("Lambda Issue", DiagCat_Lambda_Issue)
CATEGORY("Verification", DiagCat_Verification)
CATEGORY("ARC Semantic Issue", DiagCat_ARC_Semantic_Issue)
CATEGORY("ARC and @properties", DiagCat_ARC_and__properties)
CATEGORY("ARC Casting Rules", DiagCat_ARC_Casting_Rules)
CATEGORY("ARC Parse Issue", DiagCat_ARC_Parse_Issue)
CATEGORY("ARC Weak References", DiagCat_ARC_Weak_References)
CATEGORY("ARC Restrictions", DiagCat_ARC_Restrictions)
CATEGORY("OpenMP Issue", DiagCat_OpenMP_Issue)
CATEGORY("Inline Assembly Issue", DiagCat_Inline_Assembly_Issue)
CATEGORY("AST Deserialization Issue", DiagCat_AST_Deserialization_Issue)
CATEGORY("Modules Issue", DiagCat_Modules_Issue)
CATEGORY("Codegen ABI Check", DiagCat_Codegen_ABI_Check)
CATEGORY("Coroutines Issue", DiagCat_Coroutines_Issue)
CATEGORY("Command line", DiagCat_Command_line)
CATEGORY("Concepts Issue", DiagCat_Concepts_Issue)
CATEGORY("Dependency Directive Source Scanner Issue", DiagCat_Dependency_Directive_Source_Scanner_Issue)
CATEGORY("Backend Issue", DiagCat_Backend_Issue)
CATEGORY("SourceMgr Reported Issue", DiagCat_SourceMgr_Reported_Issue)
CATEGORY("API Notes Issue", DiagCat_API_Notes_Issue)
CATEGORY("Related Result Type Issue", DiagCat_Related_Result_Type_Issue)
CATEGORY("AST Serialization Issue", DiagCat_AST_Serialization_Issue)
CATEGORY("Nullability Issue", DiagCat_Nullability_Issue)
CATEGORY("Generics Issue", DiagCat_Generics_Issue)
CATEGORY("User-Defined Issue", DiagCat_User_Defined_Issue)
CATEGORY("Refactoring Invocation Issue", DiagCat_Refactoring_Invocation_Issue)
CATEGORY("VTable ABI Issue", DiagCat_VTable_ABI_Issue)
CATEGORY("Value Conversion Issue", DiagCat_Value_Conversion_Issue)
CATEGORY("Documentation Issue", DiagCat_Documentation_Issue)
CATEGORY("ARC Retain Cycle", DiagCat_ARC_Retain_Cycle)
CATEGORY("Deprecations", DiagCat_Deprecations)
CATEGORY("Format String Issue", DiagCat_Format_String_Issue)
CATEGORY("Cocoa API Issue", DiagCat_Cocoa_API_Issue)
CATEGORY("#pragma message Directive", DiagCat__pragma_message_Directive)
CATEGORY("Instrumentation Issue", DiagCat_Instrumentation_Issue)
CATEGORY("Unused Entity Issue", DiagCat_Unused_Entity_Issue)
#endif // GET_CATEGORY_TABLE

