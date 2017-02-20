// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#define tryOperation(add_or_remove, op, timeout) \
    LockPtr lock; \
    if (!this->prepareSafeTry##add_or_remove(lock, timeout)) \
        return false; \
    this->op; \
    this->finishSafe##add_or_remove(lock); \
    return true;

#define doOperation(add_or_remove, op) \
    LockPtr lock; \
    this->prepareSafe##add_or_remove(lock); \
    this->op; \
    this->finishSafe##add_or_remove(lock);

#define basicOperation(basic_template, try_template, name, tryName, add_or_remove, type, op) \
    basic_template \
    void name(type) \
    { doOperation(add_or_remove, op); } \
    basic_template \
    bool tryName(type) \
    { tryOperation(add_or_remove, op, std::try_to_lock) } \
    template <try_template class Rep_, class Period_> \
    bool tryName##For(type, const std::chrono::duration<Rep_, Period_>& timeout_duration) \
    { tryOperation(add_or_remove, op, timeout_duration) } \
    template <try_template class Clock_, class Duration_> \
    bool tryName##Until(type, const std::chrono::time_point<Clock_, Duration_>& timeout_time) \
    { tryOperation(add_or_remove, op, timeout_time) }

#define COMMA ,
#define templatedOperation(template_args, name, tryName, add_or_remove, type, op) \
    basicOperation(template<template_args>, template_args COMMA, name, tryName, add_or_remove, type, op)

#define Operation(name, tryName, add_or_remove, type, op) \
    basicOperation(, , name, tryName, add_or_remove, type, op)

#define AddOperation(name, tryName, type, op) Operation(name, tryName, Add, type, op)
#define RemoveOperation(name, tryName, type, op) Operation(name, tryName, Remove, type, op)

#define LRefAddOperation(name, tryName, op) AddOperation(name, tryName, const T_& element, op(element))
#define LRefRemoveOperation(name, tryName, op) RemoveOperation(name, tryName, T_& element, op(element))

#define RRefAddOperation(name, tryName, op) AddOperation(name, tryName, T_&& element, op(std::move(element)))
#define RRefRemoveOperation(name, tryName, op) RemoveOperation(name, tryName, T_&& element, op(std::move(element)))

#define CreateOperation(name, tryName, op) templatedOperation(class... Args_, name, tryName, Add, Args_&&... args, op(std::forward<Args_>(args)...))