// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MovieSceneToolsPrivatePCH.h"
#include "MovieSceneColorTrack.h"
#include "ColorPropertyTrackEditor.h"
#include "ColorPropertySection.h"
#include "MatineeImportTools.h"
#include "Matinee/InterpTrackColorProp.h"


FName FColorPropertyTrackEditor::RedName( "R" );
FName FColorPropertyTrackEditor::GreenName( "G" );
FName FColorPropertyTrackEditor::BlueName( "B" );
FName FColorPropertyTrackEditor::AlphaName( "A" );


TSharedRef<ISequencerTrackEditor> FColorPropertyTrackEditor::CreateTrackEditor(TSharedRef<ISequencer> InSequencer)
{
	return MakeShareable(new FColorPropertyTrackEditor(InSequencer));
}


TSharedRef<FPropertySection> FColorPropertyTrackEditor::MakePropertySectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track )
{
	return MakeShareable(new FColorPropertySection(SectionObject, GetSequencer().Get(), Track));
}


void FColorPropertyTrackEditor::GenerateKeysFromPropertyChanged( const FPropertyChangedParams& PropertyChangedParams, TArray<FColorKey>& NewGeneratedKeys, TArray<FColorKey>& DefaultGeneratedKeys )
{
	const UStructProperty* StructProp = Cast<const UStructProperty>( PropertyChangedParams.PropertyPath.Last() );
	FName StructName = StructProp->Struct->GetFName();
	FName PropertyName = PropertyChangedParams.PropertyPath.Last()->GetFName();

	bool bIsFColor = StructName == NAME_Color;
	bool bIsFLinearColor = StructName == NAME_LinearColor;
	bool bIsSlateColor = StructName == FName( "SlateColor" );

	FLinearColor ColorValue;

	if (bIsFColor)
	{
		ColorValue = FLinearColor( PropertyChangedParams.GetPropertyValue<FColor>() );
	}
	else
	{
		ColorValue = PropertyChangedParams.GetPropertyValue<FLinearColor>();
	}

	if( StructProp->HasMetaData("HideAlphaChannel") )
	{
		ColorValue.A = 1;
	}

	FName ChannelName = PropertyChangedParams.StructPropertyNameToKey;

	TArray<FColorKey>& RedKeys = ChannelName == NAME_None || ChannelName == RedName ? NewGeneratedKeys : DefaultGeneratedKeys;
	RedKeys.Add( FColorKey( EKeyColorChannel::Red, ColorValue.R, bIsSlateColor ) );

	TArray<FColorKey>& GreenKeys = ChannelName == NAME_None || ChannelName == GreenName ? NewGeneratedKeys : DefaultGeneratedKeys;
	GreenKeys.Add( FColorKey( EKeyColorChannel::Green, ColorValue.G, bIsSlateColor ) );

	TArray<FColorKey>& BlueKeys =  ChannelName == NAME_None || ChannelName == BlueName ? NewGeneratedKeys : DefaultGeneratedKeys;
	BlueKeys.Add( FColorKey( EKeyColorChannel::Blue, ColorValue.B, bIsSlateColor ) );

	TArray<FColorKey>& AlphaKeys = ChannelName == NAME_None || ChannelName == AlphaName ? NewGeneratedKeys : DefaultGeneratedKeys;
	AlphaKeys.Add( FColorKey( EKeyColorChannel::Alpha, ColorValue.A, bIsSlateColor ) );
}

void CopyInterpColorTrack(TSharedRef<ISequencer> Sequencer, UInterpTrackColorProp* ColorPropTrack, UMovieSceneColorTrack* ColorTrack)
{
	if (FMatineeImportTools::CopyInterpColorTrack(ColorPropTrack, ColorTrack))
	{
		Sequencer.Get().NotifyMovieSceneDataChanged();
	}
}

void FColorPropertyTrackEditor::BuildTrackContextMenu( FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track )
{
	UInterpTrackColorProp* ColorPropTrack = nullptr;
	for ( UObject* CopyPasteObject : GUnrealEd->MatineeCopyPasteBuffer )
	{
		ColorPropTrack = Cast<UInterpTrackColorProp>( CopyPasteObject );
		if ( ColorPropTrack != nullptr )
		{
			break;
		}
	}
	UMovieSceneColorTrack* ColorTrack = Cast<UMovieSceneColorTrack>( Track );
	MenuBuilder.AddMenuEntry(
		NSLOCTEXT( "Sequencer", "PasteMatineeTrack", "Paste Matinee Color Track" ),
		NSLOCTEXT( "Sequencer", "PasteMatineeTrackTooltip", "Pastes keys from a Matinee color track into this track." ),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateStatic( &CopyInterpColorTrack, GetSequencer().ToSharedRef(), ColorPropTrack, ColorTrack ),
			FCanExecuteAction::CreateLambda( [=]()->bool { return ColorPropTrack != nullptr && ColorPropTrack->GetNumKeys() > 0 && ColorTrack != nullptr; } ) ) );
}

