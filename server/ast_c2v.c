/******************************************************************************
  Copyright (c) 1992, 1995, 1996 Xerox Corporation.  All rights reserved.
  Portions of this code were written by Stephen White, aka ghond.
  Use and copying of this software and preparation of derivative works based
  upon this software are permitted.  Any distribution of this software or
  derivative works must comply with all applicable United States export
  control laws.  This software is made available AS IS, and Xerox Corporation
  makes no warranty about the software, its performance or its conformity to
  any specification.  Any person obtaining a copy of this software is requested
  to send their name and post office or electronic mail address to:
    Pavel Curtis
    Xerox PARC
    3333 Coyote Hill Rd.
    Palo Alto, CA 94304
    Pavel@Xerox.Com
 *****************************************************************************/

#include "my-ctype.h"
#include "my-stdio.h"

#include "ast.h"
#include "config.h"
#include "log.h"
#include "storage.h"
#include "utils.h"


typedef int *Slots;
static void const2var_stmt(Slots * slots, Stmt * stmt, int indent);
static void const2var_stmt_cond(Slots * slots, struct Stmt_Cond cond, int indent);
static void const2var_stmt_catch(Slots * slots, struct Stmt_Catch catch, int indent);
static void const2var_scatter(Slots * slots, Scatter * sc);
static void const2var_arglist(Slots * slots, Arg_List * args);
static void const2var_expr(Slots * slots, Expr * expr);

void
const2var_program(Slots slots, Stmt * top)
{
    const2var_stmt(&slots, top, 0);
}

static void
const2var_stmt_cond(Slots * slots, struct Stmt_Cond cond, int indent)
{
    Cond_Arm *elseifs;

    const2var_expr(slots, cond.arms->condition);
    const2var_stmt(slots, cond.arms->stmt, indent + 2);
    for (elseifs = cond.arms->next; elseifs; elseifs = elseifs->next) {
	const2var_expr(slots, elseifs->condition);
	const2var_stmt(slots, elseifs->stmt, indent + 2);
    }
    if (cond.otherwise) {
	const2var_stmt(slots, cond.otherwise, indent + 2);
    }
}

static void
const2var_stmt_list(Slots * slots, struct Stmt_List list, int indent)
{
    /* [list.id] */
    const2var_expr(slots, list.expr);
    const2var_stmt(slots, list.body, indent + 2);
}

static void
const2var_stmt_range(Slots * slots, struct Stmt_Range range, int indent)
{
    /* [range.id] */
    const2var_expr(slots, range.from);
    const2var_expr(slots, range.to);
    const2var_stmt(slots, range.body, indent + 2);
}

static void
const2var_stmt_fork(Slots * slots, struct Stmt_Fork fork_stmt, int indent)
{
    /* fork_stmt.id */
    const2var_expr(slots, fork_stmt.time);
    const2var_stmt(slots, fork_stmt.body, indent + 2);
}

static void
const2var_stmt_catch(Slots * slots, struct Stmt_Catch catch, int indent)
{
    Except_Arm *ex;

    const2var_stmt(slots, catch.body, indent + 2);
    for (ex = catch.excepts; ex; ex = ex->next) {
	/* [ex->id]); */
	if (ex->codes)
	    const2var_arglist(slots, ex->codes);
	else
	    /* ANY */ ;
	const2var_stmt(slots, ex->stmt, indent + 2);
    }
}

static void
const2var_stmt(Slots * slots, Stmt * stmt, int indent)
{
    while (stmt) {
	switch (stmt->kind) {
	case STMT_COND:
	    const2var_stmt_cond(slots, stmt->s.cond, indent);
	    break;
	case STMT_LIST:
	    const2var_stmt_list(slots, stmt->s.list, indent);
	    break;
	case STMT_RANGE:
	    const2var_stmt_range(slots, stmt->s.range, indent);
	    break;
	case STMT_FORK:
	    const2var_stmt_fork(slots, stmt->s.fork, indent);
	    break;
	case STMT_EXPR:
	    const2var_expr(slots, stmt->s.expr);
	    break;
	case STMT_WHILE:
	    /* stmt->s.loop.id */
	    const2var_expr(slots, stmt->s.loop.condition);
	    const2var_stmt(slots, stmt->s.loop.body, indent + 2);
	    break;
	case STMT_RETURN:
	    if (stmt->s.expr)
		const2var_expr(slots, stmt->s.expr);
	    break;
	case STMT_TRY_EXCEPT:
	    const2var_stmt_catch(slots, stmt->s.catch, indent);
	    break;
	case STMT_TRY_FINALLY:
	    const2var_stmt(slots, stmt->s.finally.body, indent + 2);
	    const2var_stmt(slots, stmt->s.finally.handler, indent + 2);
	    break;
	case STMT_BREAK:
	case STMT_CONTINUE:
	    {
		/* varslots[stmt->s.exit] */
	    }
	    break;
	default:
	    errlog("CONST2VAR_STMT: Unknown Stmt_Kind: %d\n", stmt->kind);
	    break;
	}
	stmt = stmt->next;
    }
}

static void
const2var_name_expr(Slots * slots, Expr * expr)
{
    const2var_expr(slots, expr);
}

static void
const2var_lvalue_before(Slots * slots, Expr * expr)
{
    switch (expr->kind) {
    case EXPR_RANGE:
	const2var_lvalue_before(slots, expr->e.range.base);
	const2var_expr(slots, expr->e.range.from);
	const2var_expr(slots, expr->e.range.to);
	break;
    case EXPR_INDEX:
	const2var_lvalue_before(slots, expr->e.bin.lhs);
	const2var_expr(slots, expr->e.bin.rhs);
	break;
    case EXPR_ID:
	break;
    case EXPR_PROP:
	const2var_expr(slots, expr->e.bin.lhs);
	const2var_expr(slots, expr->e.bin.rhs);
	break;
    default:
	errlog("CONST2VAR_LVALUE_BEFORE: Unknown Expr_Kind: %d\n", expr->kind);
    }
}

static void
const2var_lvalue_after(Slots * slots, Expr * expr)
{
    while (1) {
	switch (expr->kind) {
	case EXPR_RANGE:
	    expr = expr->e.range.base;
	    continue;
	case EXPR_INDEX:
	    expr = expr->e.bin.lhs;
	    continue;
	case EXPR_ID:
	    /* HERE */
	    break;
	case EXPR_PROP:
	    break;
	default:
	    errlog("CONST2VAR_LVALUE_AFTER: Unknown Expr_Kind: %d\n", expr->kind);
	}
	break;
    }
}

static void
const2var_expr(Slots * slots, Expr * expr)
{
    switch (expr->kind) {
    case EXPR_PROP:
	const2var_expr(slots, expr->e.bin.lhs);
	const2var_name_expr(slots, expr->e.bin.rhs);
	break;

    case EXPR_VERB:
	const2var_expr(slots, expr->e.verb.obj);
	const2var_name_expr(slots, expr->e.verb.verb);
	const2var_arglist(slots, expr->e.verb.args);
	break;

    case EXPR_INDEX:
	const2var_expr(slots, expr->e.bin.lhs);
	const2var_expr(slots, expr->e.bin.rhs);
	break;

    case EXPR_RANGE:
	const2var_expr(slots, expr->e.range.base);
	const2var_expr(slots, expr->e.range.from);
	const2var_expr(slots, expr->e.range.to);
	break;

	/* left-associative binary operators */
    case EXPR_PLUS:
    case EXPR_MINUS:
    case EXPR_TIMES:
    case EXPR_DIVIDE:
    case EXPR_MOD:
    case EXPR_AND:
    case EXPR_OR:
    case EXPR_EQ:
    case EXPR_NE:
    case EXPR_LT:
    case EXPR_GT:
    case EXPR_LE:
    case EXPR_GE:
    case EXPR_IN:
	const2var_expr(slots, expr->e.bin.lhs);
	const2var_expr(slots, expr->e.bin.rhs);
	break;

	/* right-associative binary operators */
    case EXPR_EXP:
	const2var_expr(slots, expr->e.bin.lhs);
	const2var_expr(slots, expr->e.bin.rhs);
	break;

    case EXPR_COND:
	const2var_expr(slots, expr->e.cond.condition);
	const2var_expr(slots, expr->e.cond.consequent);
	const2var_expr(slots, expr->e.cond.alternate);
	break;

    case EXPR_NEGATE:
	const2var_expr(slots, expr->e.expr);
	break;

    case EXPR_NOT:
	const2var_expr(slots, expr->e.expr);
	break;

    case EXPR_VAR:
	/* literal expr->e.var */
	break;

    case EXPR_ASGN:{
	    Expr *e = expr->e.bin.lhs;

	    if (e->kind == EXPR_SCATTER) {
		const2var_expr(slots, expr->e.bin.rhs);
		const2var_scatter(slots, e->e.scatter);
	    } else {
		const2var_lvalue_before(slots, expr->e.bin.lhs);
		const2var_expr(slots, expr->e.bin.rhs);
		const2var_lvalue_after(slots, expr->e.bin.lhs);
	    }
	    break;
	}

    case EXPR_CALL:
	/* expr->e.call.func */
	const2var_arglist(slots, expr->e.call.args);
	break;

    case EXPR_ID:
	/* expr->e.id */
	break;

    case EXPR_LIST:
	const2var_arglist(slots, expr->e.list);
	break;

    case EXPR_CATCH:
	const2var_expr(slots, expr->e.catch.try);
	if (expr->e.catch.codes)
	    const2var_arglist(slots, expr->e.catch.codes);
	else
	    /* ANY */ ;
	if (expr->e.catch.except) {
	    const2var_expr(slots, expr->e.catch.except);
	}
	break;

    case EXPR_LENGTH:
	break;

    case EXPR_HOT:
	const2var_expr(slots, expr->e.hot.expr);
	break;

    case EXPR_CONSTANT:
	/* expr->e.constant */
	{
	    int which_var = (*slots)[expr->e.constant];

	    if (which_var != -1) {
		expr->kind = EXPR_ID;
		expr->e.id = which_var;
	    }
	}
	break;

    default:
	errlog("CONST2VAR_EXPR: Unknown Expr_Kind: %d\n", expr->kind);
	break;
    }
}

static void
const2var_arglist(Slots * slots, Arg_List * args)
{
    while (args) {
	/* if (args->kind == ARG_SPLICE) */
	const2var_expr(slots, args->expr);
	args = args->next;
    }
}

static void
const2var_scatter(Slots * slots, Scatter * sc)
{
    while (sc) {
	switch (sc->kind) {
	case SCAT_REST:
	    /* @var */
	    /* fall thru to ... */
	case SCAT_REQUIRED:
	    /* var_names[sc->id] */
	    break;
	case SCAT_OPTIONAL:
	    /* [sc->id]); */
	    if (sc->expr) {
		const2var_expr(slots, sc->expr);
	    }
	}
	sc = sc->next;
    }
}

char rcsid_ast_c2v[] = "$Id: ast_c2v.c,v 1.1.2.2 2002-11-03 03:42:35 xplat Exp $";

/* 
 * $Log: not supported by cvs2svn $
 * Revision 1.1.2.1  2002/11/03 03:37:58  xplat
 * Initial support for keeping type constants in a global constants table
 * rather than every stack frame.
 *
 */
