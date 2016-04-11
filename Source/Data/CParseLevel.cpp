///////////////////////////////////////////////////////////
//  CParseLevel.cpp
//  A class to parse and setup a level (entity templates
//  and instances) from an XML file
//  Created on:      30-Jul-2005 14:40:00
//  Original author: LN
///////////////////////////////////////////////////////////

#include "BaseMath.h"
#include "Entity.h"
#include "CParseLevel.h"

namespace gen
{

/*---------------------------------------------------------------------------------------------
	Constructors / Destructors
---------------------------------------------------------------------------------------------*/

// Constructor initialises state variables
CParseLevel::CParseLevel( CEntityManager* entityManager )
{
	// Take copy of entity manager for creation
	m_EntityManager = entityManager;

	// File state
	m_CurrentSection = None;

	// Template state
	m_TemplateType = "";
	m_TemplateName = "";
	m_TemplateMesh = "";

	// Entity state
	m_EntityType = "";
	m_EntityName = "";
	m_Pos = CVector3::kOrigin;
	m_Rot = CVector3::kOrigin;
	m_Scale = CVector3(1.0f, 1.0f, 1.0f);
	m_SpinSpeed = 0.0f;
}


/*---------------------------------------------------------------------------------------------
	Callback Functions
---------------------------------------------------------------------------------------------*/

// Callback function called when the parser meets the start of a new element (the opening tag).
// The element name is passed as a string. The attributes are passed as a list of (C-style)
// string pairs: attribute name, attribute value. The last attribute is marked with a null name
void CParseLevel::StartElt( const string& eltName, SAttribute* attrs )
{
	// Open major file sections
	if (eltName == "Templates")
	{
		m_CurrentSection = Templates;
	}
	else if (eltName == "Entities")
	{
		m_CurrentSection = Entities;
	}

	// Different parsing depending on section currently being read
	switch (m_CurrentSection)
	{
		case Templates:
			TemplatesStartElt( eltName, attrs ); // Parse template start elements
			break;
		case Entities:
			EntitiesStartElt( eltName, attrs ); // Parse entity start elements
			break;
	}
}

// Callback function called when the parser meets the end of an element (the closing tag). The
// element name is passed as a string
void CParseLevel::EndElt( const string& eltName )
{
	// Close major file sections
	if (eltName == "Templates" || eltName == "Entities")
	{
		m_CurrentSection = None;
	}

	// Different parsing depending on section currently being read
	switch (m_CurrentSection)
	{
		case Templates:
			TemplatesEndElt( eltName ); // Parse template end elements
			break;
		case Entities:
			EntitiesEndElt( eltName ); // Parse entity end elements
			break;
	}
}


/*---------------------------------------------------------------------------------------------
	Section Parsing
---------------------------------------------------------------------------------------------*/

// Called when the parser meets the start of an element (opening tag) in the templates section
void CParseLevel::TemplatesStartElt( const string& eltName, SAttribute* attrs )
{
	// Started reading a new entity template - get type, name and mesh
	if (eltName == "EntityTemplate")
	{
		// Get attributes held in the tag
		m_TemplateType = GetAttribute( attrs, "Type" );
		m_TemplateName = GetAttribute( attrs, "Name" );
		m_TemplateMesh = GetAttribute( attrs, "Mesh" );
	}
}

// Called when the parser meets the end of an element (closing tag) in the templates section
void CParseLevel::TemplatesEndElt( const string& eltName )
{
	// Finished reading an entity template - create it using parsed data
	if (eltName == "EntityTemplate")
	{
		CreateEntityTemplate();
	}
}


// Called when the parser meets the start of an element (opening tag) in the entities section
void CParseLevel::EntitiesStartElt( const string& eltName, SAttribute* attrs )
{
	// Started reading a new entity - get type and name
	if (eltName == "Entity")
	{
		m_EntityType = GetAttribute( attrs, "Type" );
		m_EntityName = GetAttribute( attrs, "Name" );

		// Set default positions
		m_Pos = CVector3::kOrigin;
		m_Rot = CVector3::kOrigin;
		m_Scale = CVector3(1.0f, 1.0f, 1.0f);

		m_SpinSpeed = 0.0f;
	}

	// Started reading an entity position - get X,Y,Z
	if (eltName == "Position")
	{
		m_Pos.x = GetAttributeFloat( attrs, "X" );
		m_Pos.y = GetAttributeFloat( attrs, "Y" );
		m_Pos.z = GetAttributeFloat( attrs, "Z" );
	}
	// Started reading an entity rotation - get X,Y,Z
	else if (eltName == "Rotation")
	{
		if (GetAttribute( attrs, "Radians" ) == "true")
		{
			m_Rot.x = GetAttributeFloat( attrs, "X" );
			m_Rot.y = GetAttributeFloat( attrs, "Y" );
			m_Rot.z = GetAttributeFloat( attrs, "Z" );
		}
		else
		{
			m_Rot.x = ToRadians(GetAttributeFloat( attrs, "X" ));
			m_Rot.y = ToRadians(GetAttributeFloat( attrs, "Y" ));
			m_Rot.z = ToRadians(GetAttributeFloat( attrs, "Z" ));
		}
	}
	// Started reading an entity scale - get X,Y,Z
	else if (eltName == "Scale")
	{
		m_Scale.x = GetAttributeFloat( attrs, "X" );
		m_Scale.y = GetAttributeFloat( attrs, "Y" );
		m_Scale.z = GetAttributeFloat( attrs, "Z" );
	}
	else if (eltName == "Spin")
	{
		m_SpinSpeed = GetAttributeFloat( attrs, "Speed" );
	}

	// Randomising an entity position - get X,Y,Z amounts and randomise
	else if (eltName == "Randomise")
	{
		float randomX = GetAttributeFloat( attrs, "X" ) * 0.5f;
		float randomY = GetAttributeFloat( attrs, "Y" ) * 0.5f;
		float randomZ = GetAttributeFloat( attrs, "Z" ) * 0.5f;
		m_Pos.x += Random( -randomX, randomX );
		m_Pos.y += Random( -randomY, randomY );
		m_Pos.z += Random( -randomZ, randomZ );
	}
}

// Called when the parser meets the end of an element (closing tag) in the entities section
void CParseLevel::EntitiesEndElt( const string& eltName )
{
	// Finished reading entity - create it using parsed data
	if (eltName == "Entity")
	{
		CreateEntity();
	}
}


/*---------------------------------------------------------------------------------------------
	Entity Template and Instance Creation
---------------------------------------------------------------------------------------------*/

// Create an entity template using data collected from parsed XML elements
void CParseLevel::CreateEntityTemplate()
{
	// Initialise the template depending on its type

	// Generic template
	m_EntityManager->CreateTemplate( m_TemplateType, m_TemplateName, m_TemplateMesh );
}

// Create an entity using data collected from parsed XML elements
void CParseLevel::CreateEntity()
{
	// Get the type of template that this entity uses
	string templateType = m_EntityManager->GetTemplate( m_EntityType )->GetType();

	// Create a new entity of this template type
	if (templateType == "Planet")
	{
		m_EntityManager->CreatePlanet( m_EntityType, m_EntityName, m_SpinSpeed, m_Pos, m_Rot, m_Scale );
	}
	else
	{
		m_EntityManager->CreateEntity( m_EntityType, m_EntityName, m_Pos, m_Rot, m_Scale );
	}
}


} // namespace gen
