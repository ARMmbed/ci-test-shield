/* 
 * Copyright (c) 2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef CI_TEST_CONFIG_H
#define CI_TEST_CONFIG_H

#if defined(MBED_CONF_APP_DEBUG_MSG) && (MBED_CONF_APP_DEBUG_MSG != 0)
#define DEBUG_PRINTF(...) do { printf(__VA_ARGS__); } while(0)
#else
#define DEBUG_PRINTF(...) {}
#endif

#endif
