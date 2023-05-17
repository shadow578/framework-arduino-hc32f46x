#ifndef __CORE_HOOKS_H
#define __CORE_HOOKS_H

#ifdef __cplusplus
extern "C"
{
#endif

#define DEF_HOOK(name) __attribute__((weak)) extern void core_hook_##name();

    // main.cpp hooks
    DEF_HOOK(pre_setup)
    DEF_HOOK(post_setup)
    DEF_HOOK(loop)

#ifdef __cplusplus
}
#endif
#endif // __CORE_HOOKS_H
