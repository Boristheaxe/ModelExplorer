//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#include "stdafx.h"
#include "WallFoundationPropertyViewBuilder.h"

#include <RengaAPI/WallFoundation.h>

WallFoundationPropertyViewBuilder::WallFoundationPropertyViewBuilder(const PropertyManagers* pPropertyManagers, const QTranslator* pTranslator) 
  : ObjectPropertyViewBuilder(pPropertyManagers, pTranslator)
{}

PropertyList WallFoundationPropertyViewBuilder::createParametersProperties(rengaapi::ModelObject* pObject)
{
	rengaapi::WallFoundation* pWallFoundation = dynamic_cast<rengaapi::WallFoundation*>(pObject);
	PropertyList result;

	// name, offset, material, mark
	QtProperty* name = m_pPropertyManagers->m_pStringManager->addProperty(m_pTranslator->translate("wallFoundation", "name"));
	QtProperty* offset = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("wallFoundation", "offset"));
	QtProperty* material = m_pPropertyManagers->m_pStringManager->addProperty(m_pTranslator->translate("wallFoundation", "material"));
	QtProperty* mark = m_pPropertyManagers->m_pStringManager->addProperty(m_pTranslator->translate("wallFoundation", "mark"));

	m_pPropertyManagers->m_pStringManager->setValue(name, rengaStringToQString(pWallFoundation->name()));
	m_pPropertyManagers->m_pDoubleManager->setValue(offset, pWallFoundation->offset().inMillimeters());
	m_pPropertyManagers->m_pStringManager->setValue(material, getMaterialName(pWallFoundation->material()));
	m_pPropertyManagers->m_pStringManager->setValue(mark, rengaStringToQString(pWallFoundation->mark()));

	result.push_back(name);
	result.push_back(offset);
	result.push_back(material);
	result.push_back(mark);

  return result;
}


PropertyList WallFoundationPropertyViewBuilder::createQuantitiesProperties(rengaapi::ModelObject* pObject)
{
	rengaapi::WallFoundation* pWallFoundation = dynamic_cast<rengaapi::WallFoundation*>(pObject);
	PropertyList result;
	rengaapi::WallFoundationQuantities wallFoundationQuantities = pWallFoundation->quantities();

	// overallWidth, overallHeight, length, netFootprintArea, netSideArea, netVolume, mass
	QtProperty* overallWidth = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("wallFoundation", "overallWidth"));
	QtProperty* overallHeight = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("wallFoundation", "overallHeight"));
	QtProperty* length = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("wallFoundation", "length"));
	QtProperty* netFootprintArea = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("wallFoundation", "netFootprintArea"));
	QtProperty* netSideArea = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("wallFoundation", "netSideArea"));
	QtProperty* netVolume = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("wallFoundation", "netVolume"));
	QtProperty* mass = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("wallFoundation", "mass"));

	setLengthMeasureOptional(result, wallFoundationQuantities.overallWidth(), overallWidth);
	setLengthMeasureOptional(result, wallFoundationQuantities.overallHeight(), overallHeight);
	setLengthMeasureOptional(result, wallFoundationQuantities.nominalLength(), length);
	setAreaMeasureOptional(result, wallFoundationQuantities.netFootprintArea(), netFootprintArea);
	setAreaMeasureOptional(result, wallFoundationQuantities.netSideArea(), netSideArea);
	setVolumeMeasureOptional(result, wallFoundationQuantities.netVolume(), netVolume);
	setOneLayeredMass(result, pWallFoundation->material(), wallFoundationQuantities.netVolume(), mass);

  return result;
}