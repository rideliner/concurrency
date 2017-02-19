// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <functional>

namespace ride { namespace detail {

template <class F_>
class Job;

template<class Func_>
struct JobResultType;

template<class Ret_>
struct JobResultType<std::function<Ret_()>>
{ typedef Ret_ type; };

template<class Ret_>
struct JobResultType<Ret_ (*)()>
{ typedef Ret_ type; };

template<class Func_>
struct JobResultType
{ typedef typename Func_::result_type type; };

} // end namespace detail

} // end namespace ride
