//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#include "stdafx.h"
#include "DoorPropertyViewBuilder.h"

#include <RengaAPI/Door.h>

DoorPropertyViewBuilder::DoorPropertyViewBuilder(const PropertyManagers* pPropertyManagers, const QTranslator* pTranslator) 
  : ObjectPropertyViewBuilder(pPropertyManagers, pTranslator)
{}

PropertyList DoorPropertyViewBuilder::createParametersProperties(rengaapi::ModelObject* pObject)
{
	rengaapi::Door* pDoor = dynamic_cast<rengaapi::Door*>(pObject);
	PropertyList result;

	// name, offset, mark
	QtProperty* name = m_pPropertyManagers->m_pStringManager->addProperty(m_pTranslator->translate("door", "name"));
	QtProperty* offset = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("door", "offset"));
	QtProperty* mark = m_pPropertyManagers->m_pStringManager->addProperty(m_pTranslator->translate("door", "mark"));

	m_pPropertyManagers->m_pStringManager->setValue(name, rengaStringToQString(pDoor->name()));
	m_pPropertyManagers->m_pDoubleManager->setValue(offset, pDoor->offset().inMillimeters());
	m_pPropertyManagers->m_pStringManager->setValue(mark, rengaStringToQString(pDoor->mark()));

	result.push_back(name);
	result.push_back(offset);
	result.push_back(mark);

  return result;
}


PropertyList DoorPropertyViewBuilder::createQuantitiesProperties(rengaapi::ModelObject* pObject)
{
  rengaapi::Door* pDoor = dynamic_cast<rengaapi::Door*>(pObject);
	rengaapi::DoorQuantities doorQuantities = pDoor->quantities();

	PropertyList result;

  // overallHeight, overallWidth, perimeter, area
	QtProperty* overallHeight = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("door", "overallHeight"));
	QtProperty* overallWidth = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("door", "overallWidth"));
	QtProperty* perimeter = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("door", "perimeter"));
	QtProperty* area = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("door", "area"));

	setLengthMeasureOptional(result, doorQuantities.overallHeight(), overallHeight);
	setLengthMeasureOptional(result, doorQuantities.overallWidth(), overallWidth);
	setLengthMeasureOptional(result, doorQuantities.perimeter(), perimeter);
	setAreaMeasureOptional(result, doorQuantities.area(), area);

  return result;
}