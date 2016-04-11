/*******************************************
	EntityManager.h

	Responsible for entity creation and
	destruction
********************************************/

#pragma once

#include <map>
using namespace std;

#include "Defines.h"
#include "CHashTable.h"
#include "Entity.h"
#include "PlanetEntity.h"
#include "Camera.h"

namespace gen
{

// The entity manager is responsible for creation, update, rendering and deletion of
// entities. It also manages UIDs for entities using a hash table
class CEntityManager
{
/////////////////////////////////////
//	Constructors/Destructors
public:

	// Constructor
	CEntityManager();

	// Destructor
	~CEntityManager();

private:
	// Prevent use of copy constructor and assignment operator (private and not defined)
	CEntityManager( const CEntityManager& );
	CEntityManager& operator=( const CEntityManager& );


/////////////////////////////////////
//	Public interface
public:

	/////////////////////////////////////
	// Template creation / destruction

	// Create a base entity template with the given type, name and mesh. Returns the new entity
	// template pointer
	CEntityTemplate* CEntityManager::CreateTemplate
	(
		const string& type,
		const string& name,
		const string& mesh
	);

	// Note: Planets use the base template class, don't need a custom function

	// Destroy the given template (name) - returns true if the template existed and was destroyed
	bool DestroyTemplate( const string& name );

	// Destroy all templates held by the manager
	void DestroyAllTemplates();


	/////////////////////////////////////
	// Entity creation / destruction

	// Create a base class entity - requires a template name, may supply entity name and position
	// Returns the UID of the new entity
	TEntityUID CreateEntity
	(
		const string&    templateName,
		const string&    name = "",
		const CVector3&  position = CVector3::kOrigin, 
		const CVector3&  rotation = CVector3( 0.0f, 0.0f, 0.0f ),
		const CVector3&  scale = CVector3( 1.0f, 1.0f, 1.0f )
	);

	// Create a planet, requires a planet template name, may supply entity name and position
	// Returns the UID of the new entity
	TEntityUID CreatePlanet
	(
		const string&   templateName,
		const string&   name = "",
		TFloat32        spinSpeed = kfPi,
		const CVector3& position = CVector3::kOrigin, 
		const CVector3& rotation = CVector3( 0.0f, 0.0f, 0.0f ),
		const CVector3& scale = CVector3( 1.0f, 1.0f, 1.0f )
	);

	// Destroy the given entity - returns true if the entity existed and was destroyed
	bool DestroyEntity( TEntityUID UID );

	// Destroy all entities held by the manager
	void DestroyAllEntities();


	/////////////////////////////////////
	// Template / Entity access

	// Return the template with the given name
	CEntityTemplate* GetTemplate( const string& name )
	{
		// Find the template name in the template map
		TTemplateIter entityTemplate = m_Templates.find( name );
		if (entityTemplate == m_Templates.end())
		{
			// Template name not found
			return 0;
		}
		return (*entityTemplate).second;
	}


	// Return the number of entities
	TUInt32 NumEntities() 
	{
		return static_cast<TUInt32>(m_Entities.size());
	}

	// Return the entities at the given array index
	CEntity* GetEntityAtIndex( TUInt32 index )
	{
		return m_Entities[index];
	}

	// Return the entity with the given UID
	CEntity* GetEntity( TEntityUID UID )
	{
		// Find the entity UID in the entity hash map
		TUInt32 entityIndex;
		if (!m_EntityUIDMap->LookUpKey( UID, &entityIndex ))
		{
			return 0;
		}
		return m_Entities[entityIndex];
	}

	// Return the entity with the given name & optionally the given template name & template type
	CEntity* GetEntity( const string& name, const string& templateName = "",
	                    const string& templateType = "" )
	{
		TEntityIter entity = m_Entities.begin();
		while (entity != m_Entities.end())
		{
			if ((*entity)->GetName() == name && 
				(templateName.length() == 0 || (*entity)->Template()->GetName() == templateName) &&
				(templateType.length() == 0 || (*entity)->Template()->GetType() == templateType))
			{
				return (*entity);
			}
			++entity;
		}
		return 0;
	}


	// Begin an enumeration of entities matching given name, template name and template type
	// An empty string indicates to match anything in this field (would be nice to support
	// wildcards, e.g. match name of "Ship*")
	void BeginEnumEntities( const string& name, const string& templateName,
	                        const string& templateType = "" )
	{
		m_IsEnumerating = true;
		m_EnumEntity = m_Entities.begin();
		m_EnumName = name;
		m_EnumTemplateName = templateName;
		m_EnumTemplateType = templateType;
	}

	// Finish enumerating entities (see above)
	void EndEnumEntities()
	{
		m_IsEnumerating = false;
	}

	// Return next entity matching parameters passed to a previous call to BeginEnumEntities
	// Returns 0 if BeginEnumEntities not called or no more matching entities
	CEntity* EnumEntity()
	{
		if (!m_IsEnumerating)
		{
			return 0;
		}

		while (m_EnumEntity != m_Entities.end())
		{
			if ((m_EnumName.length() == 0 || (*m_EnumEntity)->GetName() == m_EnumName) && 
				(m_EnumTemplateName.length() == 0 ||
				 (*m_EnumEntity)->Template()->GetName() == m_EnumTemplateName) &&
				(m_EnumTemplateType.length() == 0 ||
				 (*m_EnumEntity)->Template()->GetType() == m_EnumTemplateType))
			{
				CEntity* foundEntity = *m_EnumEntity;
				++m_EnumEntity;
				return foundEntity;
			}
			++m_EnumEntity;
		}
		
		m_IsEnumerating = false;
		return 0;
	}


	/////////////////////////////////////
	// Update / Rendering

	// Call all entity update functions - not the ideal method, OK for this example
	// Pass the time since last update
	void UpdateAllEntities( float updateTime );

	// Render all entities from point of view of given camera - not the ideal method, OK for this example
	// May request to render either normal or post-processed materials in the entities (defaults to normal)
	void RenderAllEntities( CCamera* camera, bool postProcess = false );

		
/////////////////////////////////////
//	Private interface
private:

	/////////////////////////////////////
	// Types

	// Entity templates are held in a map, define some types for convenience
	typedef map<string, CEntityTemplate*> TTemplates;
	typedef TTemplates::iterator TTemplateIter;

	// Entity instances are held in a vector, define some types for convenience
	typedef vector<CEntity*> TEntities;
	typedef TEntities::iterator TEntityIter;


	/////////////////////////////////////
	// Template Data

	// The map of template names / templates
	TTemplates m_Templates;


	/////////////////////////////////////
	// Entity Data

	// The main list of entities. This vector is kept packed - i.e. with no gaps. If an
	// entity is removed from the middle of the list, the last entity is moved down to
	// fill its space
	TEntities m_Entities;

	// A mapping from UIDs to indexes into the above array
	CHashTable<TEntityUID, TUInt32>* m_EntityUIDMap;

	// Entity IDs are provided using a single increasing integer
	TEntityUID m_NextUID;


	/////////////////////////////////////
	// Data for Entity Enumeration

	bool        m_IsEnumerating;
	TEntityIter m_EnumEntity;
	string      m_EnumName;
	string      m_EnumTemplateName;
	string      m_EnumTemplateType;
};


} // namespace gen
