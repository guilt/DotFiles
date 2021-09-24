/*
 * CTK - The Cell Tool Kit Library
 * http://ctk-dev.sourceforge.net/
 * http://cell.fixstars.com/ctk/
 *
 * Copyright (C) 2006-2008 Fixstars Corp.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * Internal templates for SpeFunction.
 */
#if !defined(_CTK_SPE_FUNCTION_HPP) && !defined(CTK_PP_ITERATION)
# error do not include this file directly!
#endif

/* for SpeFunction params/args */
#if CTK_FUNCTION_NUM_ARGS == 0
# define CTK_FUNCTION_COMMA
# define CTK_MAP_FIRST_ARG
# define CTK_MAP_LOOP_COND  
#else
# define CTK_FUNCTION_COMMA ,
# define CTK_MAP_FIRST_ARG  Iterator0 first0, Iterator0 last0
# define CTK_MAP_LOOP_COND  && first0 != last0
#endif

#define CTK_FUNCTION_PARM(n, x)      CTK_PP_CAT(T, n) CTK_PP_CAT(a, n)
#define CTK_FUNCTION_MARSHAL(n, x)   SpeFunctionArg<CTK_PP_CAT(T, n)>::marshal(argp, CTK_PP_CAT(a, n))

#define CTK_MAP_ARG(n, x)            CTK_PP_CAT(Iterator, n) CTK_PP_CAT(first, n)
#define CTK_MAP_FUNC_ARG(n, x)       CTK_PP_CAT(*first, n)
#define CTK_MAP_INCREMENT(n, x)      CTK_PP_INC(CTK_PP_CAT(first, n))

#define CTK_FUNCTION_TEMPLATE_PARMS  CTK_PP_ENUM_PARMS(CTK_FUNCTION_NUM_ARGS, typename T)
#define CTK_FUNCTION_TEMPLATE_ARGS   CTK_PP_ENUM_PARMS(CTK_FUNCTION_NUM_ARGS, T)
#define CTK_FUNCTION_PARMS           CTK_PP_REPEAT(CTK_FUNCTION_NUM_ARGS, CTK_FUNCTION_PARM, CTK_PP_COMMA, )
#define CTK_FUNCTION_ARGS            CTK_PP_ENUM_PARMS(CTK_FUNCTION_NUM_ARGS, a)
#define CTK_FUNCTION_MARSHAL_ARGS    CTK_PP_REPEAT(CTK_FUNCTION_NUM_ARGS, CTK_FUNCTION_MARSHAL, CTK_PP_SEMICOLON, )

#define CTK_MAP_TEMPLATE_PARMS       CTK_PP_ENUM_PARMS(CTK_FUNCTION_NUM_ARGS, typename Iterator)
#define CTK_MAP_ARGS                 CTK_PP_REPEAT_1ORIGIN(CTK_FUNCTION_NUM_ARGS, CTK_MAP_ARG, CTK_PP_COMMA, )
#define CTK_MAP_FUNC_ARGS            CTK_PP_REPEAT(CTK_FUNCTION_NUM_ARGS, CTK_MAP_FUNC_ARG, CTK_PP_COMMA, )
#define CTK_MAP_LOOP_INCREMENTS      CTK_PP_REPEAT(CTK_FUNCTION_NUM_ARGS, CTK_MAP_INCREMENT, CTK_PP_SEMICOLON, )

/* for SpeFunction constructors */
#define CTK_FUNCTION_DECLARE_CONSTRUCTOR(program_type, program)    \
        SpeFunction(SpeRuntime &runtime, program_type program) : \
            m_runtime(&runtime), m_task(program) { \
            m_argp = ::ctk_malloc_align(128, 128); \
        }

#define CTK_FUNCTION_DECLARE_CONSTRUCTORS()     \
        CTK_FUNCTION_DECLARE_CONSTRUCTOR(ctk_spe_program_t &, program)  \
        CTK_FUNCTION_DECLARE_CONSTRUCTOR(const char *, fname)   \
        CTK_FUNCTION_DECLARE_CONSTRUCTOR(SpeTaskImage &, image)   \
        CTK_FUNCTION_DECLARE_CONSTRUCTOR(SpeImage &, image) \
        SpeFunction() :  m_runtime(NULL) { \
            m_argp = ::ctk_malloc_align(128, 128); \
        } \
        /** Copy constructor */ \
        SpeFunction(const SpeFunction& func) :  \
            m_runtime(func.m_runtime), m_task(func.m_task) { \
            m_argp = ::ctk_malloc_align(128, 128); \
        } \
        \
        /** Assignment operator */ \
        SpeFunction& operator=(const SpeFunction &func) { \
            if (m_task == func.m_task) \
                return *this; \
            m_runtime = func.m_runtime; \
            m_task = func.m_task; \
            m_argp = ::ctk_malloc_align(128, 128); \
            return *this;  \
        }

/* SPE function implementation */
template <typename R CTK_FUNCTION_COMMA CTK_FUNCTION_TEMPLATE_PARMS>
class SpeFunction <R (CTK_FUNCTION_TEMPLATE_ARGS)> {
        SpeRuntime *m_runtime;
        SpeTask m_task;
        void *m_argp;

    public:
        CTK_FUNCTION_DECLARE_CONSTRUCTORS()
        int getNumberOfRuntimeSpe() { return m_runtime->getNumberOfSpe(); }

        void invoke(CTK_FUNCTION_PARMS) {
            char *argp;
            argp = (char*)m_argp;
            CTK_FUNCTION_MARSHAL_ARGS;
            m_runtime->enqTask(m_task, m_argp);
        }

        R join() {
            m_runtime->waitTask(m_task);
            return m_task.getReturnValue<R>();
        }

        R operator ()(CTK_FUNCTION_PARMS) {
            invoke(CTK_FUNCTION_ARGS);
            return join();
        }
};

/* SPE function implementation (void return) */
template <CTK_FUNCTION_TEMPLATE_PARMS>
class SpeFunction <void (CTK_FUNCTION_TEMPLATE_ARGS)> {
        SpeRuntime *m_runtime;
        SpeTaskImage m_image;
        SpeTask m_task;
        void *m_argp;

    public:
        CTK_FUNCTION_DECLARE_CONSTRUCTORS()
        int getNumberOfRuntimeSpe() { return m_runtime->getNumberOfSpe(); }

        void invoke(CTK_FUNCTION_PARMS) {
            char *argp;
            argp = (char*)m_argp;
            CTK_FUNCTION_MARSHAL_ARGS;
            m_runtime->enqTask(m_task, m_argp);
        }

        void join() {
            m_runtime->waitTask(m_task);
        }

        void operator ()(CTK_FUNCTION_PARMS) {
            invoke(CTK_FUNCTION_ARGS);
            join();
        }
};

/* local function */
template <typename R CTK_FUNCTION_COMMA CTK_FUNCTION_TEMPLATE_PARMS>
struct Function <R (CTK_FUNCTION_TEMPLATE_ARGS)> {
    typedef R (&functor_type)(CTK_FUNCTION_PARMS);
    typedef R (*functor_ptr_type)(CTK_FUNCTION_PARMS);

    functor_ptr_type f;
    Function(functor_type func) : f(&func) { }

    R operator ()(CTK_FUNCTION_PARMS) {
        return f(CTK_FUNCTION_ARGS);
    }
};

/* local function (void return) */
template <CTK_FUNCTION_TEMPLATE_PARMS>
struct Function <void (CTK_FUNCTION_TEMPLATE_ARGS)> {
    typedef void (&functor_type)(CTK_FUNCTION_PARMS);
    typedef void (*functor_ptr_type)(CTK_FUNCTION_PARMS);

    functor_ptr_type f;
    Function(functor_type func) : f(&func) { }

    void operator ()(CTK_FUNCTION_PARMS) {
        f(CTK_FUNCTION_ARGS);
    }
};

/** Map template (for now does not return Function type, so Map
    cannot be nested!) */
template <class Function, typename RIterator
            CTK_FUNCTION_COMMA CTK_MAP_TEMPLATE_PARMS>
static void Map(CTK_MAP_FIRST_ARG
         CTK_MAP_ARGS CTK_FUNCTION_COMMA
         RIterator rfirst, RIterator rlast, Function func)
{
    RIterator rarg, ri = rfirst;
    std::vector<Function*> funcs(func.getNumberOfRuntimeSpe());
    typename std::vector<Function*>::iterator f = funcs.begin();
    for (size_t i = 0; i < funcs.size() && ri != rlast; ++i, ++ri) {
        funcs[i] = new Function(func);
        funcs[i]->invoke(CTK_MAP_FUNC_ARGS);
        CTK_MAP_LOOP_INCREMENTS;
    }
    rarg = ri;
    ri = rfirst;
    while (ri != rlast) {
        for (size_t i = 0; i < funcs.size() && ri != rlast; ++i, ++ri) {
            *ri = funcs[i]->join();
            if (rarg != rlast CTK_MAP_LOOP_COND) {
                funcs[i]->invoke(CTK_MAP_FUNC_ARGS);
                CTK_MAP_LOOP_INCREMENTS;
                rarg++;
            } else {
                delete funcs[i];
            }
        }
    }
    return;
}

#undef CTK_MAP_FIRST_ARG
#undef CTK_MAP_LOOP_COND
#undef CTK_FUNCTION_COMMA
#undef CTK_FUNCTION_PARM
#undef CTK_MAP_ARG
#undef CTK_MAP_FUNC_ARG
#undef CTK_MAP_INCREMENT
#undef CTK_FUNCTION_MARSHAL
#undef CTK_FUNCTION_TEMPLATE_PARMS
#undef CTK_FUNCTION_TEMPLATE_ARGS
#undef CTK_FUNCTION_MARSHAL_ARGS
#undef CTK_MAP_TEMPLATE_PARMS
#undef CTK_MAP_FUNC_ARGS
#undef CTK_MAP_LOOP_INCREMENTS
#undef CTK_FUNCTION_PARMS
#undef CTK_FUNCTION_ARGS
#undef CTK_FUNCTION_DECLARE_CONSTRUCTOR
#undef CTK_FUNCTION_DECLARE_CONSTRUCTORS
