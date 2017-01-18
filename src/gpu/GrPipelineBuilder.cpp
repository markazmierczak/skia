/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "GrPipelineBuilder.h"

#include "GrBlend.h"
#include "GrPaint.h"
#include "GrPipeline.h"
#include "GrProcOptInfo.h"
#include "GrXferProcessor.h"
#include "effects/GrPorterDuffXferProcessor.h"
#include "ops/GrOp.h"

GrPipelineBuilder::GrPipelineBuilder(GrPaint&& paint, GrAAType aaType)
        : fFlags(0x0)
        , fUserStencilSettings(&GrUserStencilSettings::kUnused)
        , fDrawFace(GrDrawFace::kBoth) {
    for (int i = 0; i < paint.numColorFragmentProcessors(); ++i) {
        fColorFragmentProcessors.emplace_back(paint.fColorFragmentProcessors[i].release());
    }

    for (int i = 0; i < paint.numCoverageFragmentProcessors(); ++i) {
        fCoverageFragmentProcessors.emplace_back(paint.fCoverageFragmentProcessors[i].release());
    }

    fXPFactory = paint.getXPFactory();

    this->setState(GrPipelineBuilder::kHWAntialias_Flag, GrAATypeIsHW(aaType));
    this->setState(GrPipelineBuilder::kDisableOutputConversionToSRGB_Flag,
                   paint.getDisableOutputConversionToSRGB());
    this->setState(GrPipelineBuilder::kAllowSRGBInputs_Flag,
                   paint.getAllowSRGBInputs());
    this->setState(GrPipelineBuilder::kUsesDistanceVectorField_Flag,
                   paint.usesDistanceVectorField());
}

bool GrPipelineBuilder::willXPNeedDstTexture(const GrCaps& caps,
                                             const GrPipelineAnalysis& analysis) const {
    if (this->getXPFactory()) {
        return this->getXPFactory()->willNeedDstTexture(caps, analysis);
    }
    return GrPorterDuffXPFactory::SrcOverWillNeedDstTexture(caps, analysis);
}
