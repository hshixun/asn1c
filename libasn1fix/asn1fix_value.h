/*
 * Functions related with processing values.
 */
#ifndef	_ASN1FIX_VALUE_H_
#define	_ASN1FIX_VALUE_H_

/*
 * Resolve the value given by reference.
 * This function also takes a parameter which specifies the desired
 * value's type.
 * 
 * RETURN VALUES:
 * 0:		Value resolved successfully.
 * -1/EPERM:	Recursive looping detected.
 * -1/EEXIST:	Reference is not compatible with the desired type.
 * -1/ESRCH:	Cannot find the terminal reference.
 */
int asn1f_value_resolve(arg_t *arg, asn1p_expr_t *tc);

/*
 * Check if a value in value_expr refers to the enumeration or integer element
 * within the type provided. If yes, it will replace referenced value with
 * the appropriate inline value.
 */
int asn1f_look_value_in_type(arg_t *arg,
		asn1p_expr_t *type_expr, asn1p_expr_t *value_expr);

#endif	/* _ASN1FIX_VALUE_H_ */
