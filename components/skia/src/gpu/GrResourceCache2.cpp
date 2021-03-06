
/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#include "GrResourceCache2.h"
#include "GrGpuResource.h"

GrResourceCache2::~GrResourceCache2() {
    this->releaseAll();
}

void GrResourceCache2::insertResource(GrGpuResource* resource) {
    SkASSERT(NULL != resource);
    SkASSERT(!resource->wasDestroyed());
    SkASSERT(!this->isInCache(resource));
    fResources.addToHead(resource);
    ++fCount;
}

void GrResourceCache2::removeResource(GrGpuResource* resource) {
    SkASSERT(this->isInCache(resource));
    fResources.remove(resource);
    --fCount;
}

void GrResourceCache2::abandonAll() {
    while (GrGpuResource* head = fResources.head()) {
        SkASSERT(!head->wasDestroyed());
        head->abandon();
        // abandon should have already removed this from the list.
        SkASSERT(head != fResources.head());
    }
    SkASSERT(!fCount);
}

void GrResourceCache2::releaseAll() {
    while (GrGpuResource* head = fResources.head()) {
        SkASSERT(!head->wasDestroyed());
        head->release();
        // release should have already removed this from the list.
        SkASSERT(head != fResources.head());
    }
    SkASSERT(!fCount);
}
