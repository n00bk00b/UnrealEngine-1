// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	SMessagingEndpointDetails.h: Declares the SMessagingEndpointDetails class.
=============================================================================*/

#pragma once


/**
* Implements the message types panel.
*/
class SMessagingEndpointDetails
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SMessagingEndpointDetails) { }
	SLATE_END_ARGS()

public:

	/**
	 * Construct this widget
	 *
	 * @param InArgs The declaration data for this widget.
	 * @param InModel The view model to use.
	 * @param InStyle The visual style to use for this widget.
	 */
	void Construct( const FArguments& InArgs, const FMessagingDebuggerModelRef& InModel, const TSharedRef<ISlateStyle>& InStyle );

public:

	// Begin SCompoundWidget overrides

	virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) OVERRIDE;

	// End SCompoundWidget overrides

protected:

	/**
	 * Refreshes the endpoint's address information.
	 */
	void RefreshAddressInfo( );

private:

	// Callback for generating a row widget for the address list view.
	TSharedRef<ITableRow> HandleAddressListGenerateRow( FMessageTracerAddressInfoPtr AddressInfo, const TSharedRef<STableViewBase>& OwnerTable );

	// Callback for getting the number of received messages.
	FString HandleEndpointDetailsReceivedMessagesText( ) const;

	// Callback for getting the number of sent messages.
	FString HandleEndpointDetailsSentMessagesText( ) const;

private:

	// Holds the list of address information.
	TArray<FMessageTracerAddressInfoPtr> AddressList;

	// Holds the address information list view.
	TSharedPtr<SListView<FMessageTracerAddressInfoPtr> > AddressListView;

	// Holds a pointer to the view model.
	FMessagingDebuggerModelPtr Model;

	// Holds the widget's visual style.
	TSharedPtr<ISlateStyle> Style;
};
