// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "SCurveEditor.h"

class UAnimGraphNode_Trail;

class FAnimTrailNodeDetails : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;

	struct FTrailRelaxCurveEditor: public FCurveOwnerInterface
	{
		FTrailRelaxCurveEditor(UAnimGraphNode_Trail* InGraphNode = nullptr, TSharedPtr<IPropertyHandle> InTrailRelaxCurveHandle = TSharedPtr<IPropertyHandle>());
		/** FCurveOwnerInterface interface */
		virtual TArray<FRichCurveEditInfoConst> GetCurves() const override;
		virtual TArray<FRichCurveEditInfo> GetCurves() override;
		virtual void ModifyOwner() override;
		virtual void MakeTransactional() override;
		virtual void OnCurveChanged(const TArray<FRichCurveEditInfo>& ChangedCurveEditInfos) override { }
		virtual bool IsValidCurve(FRichCurveEditInfo CurveInfo) override;

	private:
		UAnimGraphNode_Trail* GraphNodeOwner;
		TSharedPtr<IPropertyHandle> TrailRelaxCurveHandle;

	} TrailRelaxCurveEditor;
};

