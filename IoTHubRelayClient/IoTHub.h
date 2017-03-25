#pragma once
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_H
#define IOTHUB_H

#ifdef __cplusplus

static const int RelayPin = 5; //D1 - https://www.wemos.cc/product/relay-shield.html

extern "C" {
#endif

	void IoTHubLoop(void);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_H */