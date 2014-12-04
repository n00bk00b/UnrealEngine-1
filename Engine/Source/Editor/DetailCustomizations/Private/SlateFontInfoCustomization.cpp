// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "DetailCustomizationsPrivatePCH.h"
#include "SlateFontInfoCustomization.h"
#include "AssetData.h"
#include "Engine/Font.h"

#define LOCTEXT_NAMESPACE "SlateFontInfo"

TSharedRef<IPropertyTypeCustomization> FSlateFontInfoStructCustomization::MakeInstance() 
{
	return MakeShareable(new FSlateFontInfoStructCustomization());
}

void FSlateFontInfoStructCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& InHeaderRow, IPropertyTypeCustomizationUtils& InStructCustomizationUtils)
{
	StructPropertyHandle = InStructPropertyHandle;

	FontObjectProperty = InStructPropertyHandle->GetChildHandle(FName("FontObject"));
	check(FontObjectProperty.IsValid());

	TypefaceFontNameProperty = InStructPropertyHandle->GetChildHandle(FName("TypefaceFontName"));
	check(TypefaceFontNameProperty.IsValid());

	FontSizeProperty = InStructPropertyHandle->GetChildHandle(FName("Size"));
	check(FontSizeProperty.IsValid());

	InHeaderRow
	.NameContent()
	[
		InStructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(250.0f)
	.MaxDesiredWidth(0.0f)
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Center)
	[
		SNew(SHorizontalBox)

		+SHorizontalBox::Slot()
		.FillWidth(2)
		[
			SNew(SObjectPropertyEntryBox)
			.PropertyHandle(FontObjectProperty)
			.AllowedClass(UFont::StaticClass())
			.OnShouldFilterAsset(FOnShouldFilterAsset::CreateStatic(&FSlateFontInfoStructCustomization::OnFilterFontAsset))
			.OnObjectChanged(this, &FSlateFontInfoStructCustomization::OnFontChanged)
			.DisplayUseSelected(false)
			.DisplayBrowse(false)
		]

		+SHorizontalBox::Slot()
		.FillWidth(2)
		.VAlign(VAlign_Center)
		[
			SAssignNew(FontEntryCombo, SComboBox<TSharedPtr<FName>>)
			.OptionsSource(&FontEntryComboData)
			.IsEnabled(this, &FSlateFontInfoStructCustomization::IsFontEntryComboEnabled)
			.OnComboBoxOpening(this, &FSlateFontInfoStructCustomization::OnFontEntryComboOpening)
			.OnSelectionChanged(this, &FSlateFontInfoStructCustomization::OnFontEntrySelectionChanged)
			.OnGenerateWidget(this, &FSlateFontInfoStructCustomization::MakeFontEntryWidget)
			[
				SNew(STextBlock)
				.Text(this, &FSlateFontInfoStructCustomization::GetFontEntryComboText)
				.Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
			]
		]

		+SHorizontalBox::Slot()
		.FillWidth(1)
		.VAlign(VAlign_Center)
		.Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
		[
			SNew(SProperty, FontSizeProperty)
			.ShouldDisplayName(false)
		]
	];
}

void FSlateFontInfoStructCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& InStructBuilder, IPropertyTypeCustomizationUtils& InStructCustomizationUtils)
{
}

bool FSlateFontInfoStructCustomization::OnFilterFontAsset(const FAssetData& InAssetData)
{
	// We want to filter font assets that aren't valid to use with Slate/UMG
	return Cast<const UFont>(InAssetData.GetAsset())->FontCacheType != EFontCacheType::Runtime;
}

void FSlateFontInfoStructCustomization::OnFontChanged(const FAssetData& InAssetData)
{
	const UFont* const FontAsset = Cast<const UFont>(InAssetData.GetAsset());
	const FName FirstFontName = (FontAsset && FontAsset->CompositeFont.DefaultTypeface.Fonts.Num()) ? FontAsset->CompositeFont.DefaultTypeface.Fonts[0].Name : NAME_None;

	TArray<FSlateFontInfo*> SlateFontInfoStructs = GetFontInfoBeingEdited();
	for(FSlateFontInfo* FontInfo : SlateFontInfoStructs)
	{
		// The font has been updated in the editor, so clear the non-UObject pointer so that the two don't conflict
		FontInfo->CompositeFont.Reset();

		// We've changed (or cleared) the font asset, so make sure and update the typeface entry name being used by the font info
		TypefaceFontNameProperty->SetValue(FirstFontName);
	}
}

bool FSlateFontInfoStructCustomization::IsFontEntryComboEnabled() const
{
	TArray<const FSlateFontInfo*> SlateFontInfoStructs = GetFontInfoBeingEdited();
	if(SlateFontInfoStructs.Num() == 0)
	{
		return false;
	}

	const FSlateFontInfo* const FirstSlateFontInfo = SlateFontInfoStructs[0];
	const UFont* const FontObject = Cast<const UFont>(FirstSlateFontInfo->FontObject);
	if(!FontObject)
	{
		return false;
	}

	// Only let people pick an entry if every struct being edited is using the same font object
	for(int32 FontInfoIndex = 1; FontInfoIndex < SlateFontInfoStructs.Num(); ++FontInfoIndex)
	{
		const FSlateFontInfo* const OtherSlateFontInfo = SlateFontInfoStructs[FontInfoIndex];
		const UFont* const OtherFontObject = Cast<const UFont>(OtherSlateFontInfo->FontObject);
		if(FontObject != OtherFontObject)
		{
			return false;
		}
	}

	return true;
}

void FSlateFontInfoStructCustomization::OnFontEntryComboOpening()
{
	TArray<FSlateFontInfo*> SlateFontInfoStructs = GetFontInfoBeingEdited();

	FontEntryComboData.Empty();

	if(SlateFontInfoStructs.Num() > 0)
	{
		const FSlateFontInfo* const FirstSlateFontInfo = SlateFontInfoStructs[0];
		const UFont* const FontObject = Cast<const UFont>(FirstSlateFontInfo->FontObject);
		check(FontObject);

		const FName ActiveFontEntry = GetActiveFontEntry();
		TSharedPtr<FName> SelectedNamePtr;

		for(const FTypefaceEntry& TypefaceEntry : FontObject->CompositeFont.DefaultTypeface.Fonts)
		{
			TSharedPtr<FName> NameEntryPtr = MakeShareable(new FName(TypefaceEntry.Name));
			FontEntryComboData.Add(NameEntryPtr);

			if(!TypefaceEntry.Name.IsNone() && TypefaceEntry.Name == ActiveFontEntry)
			{
				SelectedNamePtr = NameEntryPtr;
			}
		}

		FontEntryComboData.Sort([](const TSharedPtr<FName>& One, const TSharedPtr<FName>& Two) -> bool
		{
			return One->ToString() < Two->ToString();
		});

		FontEntryCombo->ClearSelection();
		FontEntryCombo->RefreshOptions();
		FontEntryCombo->SetSelectedItem(SelectedNamePtr);
	}
	else
	{
		FontEntryCombo->ClearSelection();
		FontEntryCombo->RefreshOptions();
	}
}

void FSlateFontInfoStructCustomization::OnFontEntrySelectionChanged(TSharedPtr<FName> InNewSelection, ESelectInfo::Type)
{
	if(InNewSelection.IsValid())
	{
		TArray<FSlateFontInfo*> SlateFontInfoStructs = GetFontInfoBeingEdited();
		if(SlateFontInfoStructs.Num() > 0)
		{
			const FSlateFontInfo* const FirstSlateFontInfo = SlateFontInfoStructs[0];
			if(FirstSlateFontInfo->TypefaceFontName != *InNewSelection)
			{
				TypefaceFontNameProperty->SetValue(*InNewSelection);
			}
		}
	}
}

TSharedRef<SWidget> FSlateFontInfoStructCustomization::MakeFontEntryWidget(TSharedPtr<FName> InFontEntry)
{
	return
		SNew(STextBlock)
		.Text(FText::FromName(*InFontEntry))
		.Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
}

FText FSlateFontInfoStructCustomization::GetFontEntryComboText() const
{
	return FText::FromName(GetActiveFontEntry());
}

FName FSlateFontInfoStructCustomization::GetActiveFontEntry() const
{
	TArray<const FSlateFontInfo*> SlateFontInfoStructs = GetFontInfoBeingEdited();
	if(SlateFontInfoStructs.Num() > 0)
	{
		const FSlateFontInfo* const FirstSlateFontInfo = SlateFontInfoStructs[0];
		const UFont* const FontObject = Cast<const UFont>(FirstSlateFontInfo->FontObject);
		if(FontObject)
		{
			return (FirstSlateFontInfo->TypefaceFontName.IsNone() && FontObject->CompositeFont.DefaultTypeface.Fonts.Num())
				? FontObject->CompositeFont.DefaultTypeface.Fonts[0].Name
				: FirstSlateFontInfo->TypefaceFontName;
		}
	}

	return NAME_None;
}

TArray<FSlateFontInfo*> FSlateFontInfoStructCustomization::GetFontInfoBeingEdited()
{
	TArray<FSlateFontInfo*> SlateFontInfoStructs;

	if(StructPropertyHandle->IsValidHandle())
	{
		TArray<void*> StructPtrs;
		StructPropertyHandle->AccessRawData(StructPtrs);
		SlateFontInfoStructs.Reserve(StructPtrs.Num());

		for(auto It = StructPtrs.CreateConstIterator(); It; ++It)
		{
			void* RawPtr = *It;
			if(RawPtr)
			{
				FSlateFontInfo* const SlateFontInfo = reinterpret_cast<FSlateFontInfo*>(RawPtr);
				SlateFontInfoStructs.Add(SlateFontInfo);
			}
		}
	}

	return SlateFontInfoStructs;
}

TArray<const FSlateFontInfo*> FSlateFontInfoStructCustomization::GetFontInfoBeingEdited() const
{
	TArray<const FSlateFontInfo*> SlateFontInfoStructs;

	if(StructPropertyHandle->IsValidHandle())
	{
		TArray<const void*> StructPtrs;
		StructPropertyHandle->AccessRawData(StructPtrs);
		SlateFontInfoStructs.Reserve(StructPtrs.Num());

		for(auto It = StructPtrs.CreateConstIterator(); It; ++It)
		{
			const void* RawPtr = *It;
			if(RawPtr)
			{
				const FSlateFontInfo* const SlateFontInfo = reinterpret_cast<const FSlateFontInfo*>(RawPtr);
				SlateFontInfoStructs.Add(SlateFontInfo);
			}
		}
	}

	return SlateFontInfoStructs;
}

#undef LOCTEXT_NAMESPACE
