//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#include "RoofPropertyViewBuilder.h"
#include "RengaStringToQString.h"

#include <RengaAPI/Roof.h>

RoofPropertyViewBuilder::RoofPropertyViewBuilder(const PropertyManagers* pPropertyManagers, const QTranslator* pTranslator) 
  : ObjectPropertyViewBuilder(pPropertyManagers, pTranslator)
{}

PropertyMap RoofPropertyViewBuilder::createParametersProperties(rengaapi::ModelObject* pObject)
{
	rengaapi::Roof* pRoof = dynamic_cast<rengaapi::Roof*>(pObject);
	PropertyMap result;

	// name, offset, material, mark
	QtProperty* name = m_pPropertyManagers->m_pStringManager->addProperty(m_pTranslator->translate("roof", "name"));
	QtProperty* offset = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("roof", "offset"));
	QtProperty* material = m_pPropertyManagers->m_pStringManager->addProperty(m_pTranslator->translate("roof", "material"));
	QtProperty* mark = m_pPropertyManagers->m_pStringManager->addProperty(m_pTranslator->translate("roof", "mark"));

	m_pPropertyManagers->m_pStringManager->setValue(name, rengaStringToQString(pRoof->name()));

  m_pPropertyManagers->m_pDoubleManager->setValue(offset, pRoof->offset().inMillimeters());
	m_pPropertyManagers->m_pStringManager->setValue(material, getMaterialName(pRoof->material()));
	m_pPropertyManagers->m_pStringManager->setValue(mark, rengaStringToQString(pRoof->mark()));

	result.insert(std::make_pair(name->propertyName(), name));
	result.insert(std::make_pair(offset->propertyName(), offset));
	result.insert(std::make_pair(material->propertyName(), material));
	result.insert(std::make_pair(mark->propertyName(), mark));

  return result;
}


PropertyMap RoofPropertyViewBuilder::createQuantitiesProperties(rengaapi::ModelObject* pObject)
{
	rengaapi::Roof* pRoof = dynamic_cast<rengaapi::Roof*>(pObject);
	PropertyMap result;

	rengaapi::RoofQuantities roofQuantities = pRoof->quantities();
	rengaapi::RoofMaterialLayerQuantityCollection roofMaterialLayerQuantityCollection = roofQuantities.materialQuantities();
	std::vector<rengabase::VolumeMeasureOptional> volumeCollection;
	for (size_t i = 0 ; i < roofMaterialLayerQuantityCollection.size() ; ++i)
  {
		volumeCollection.push_back(roofMaterialLayerQuantityCollection.get(i).netVolume());
  }

	// thickness, netVolume, totalSurfaceArea, mass
  QtProperty* thickness = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("roof", "thickness"));
	QtProperty* netVolume = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("roof", "netVolume"));
	QtProperty* totalSurfaceArea = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("roof", "totalSurfaceArea"));
	QtProperty* mass = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("roof", "mass"));

  setLengthMeasureOptional(result, roofQuantities.nominalThickness(), thickness);
	setVolumeMeasureOptional(result, roofQuantities.netVolume(), netVolume);
	setAreaMeasureOptional(result, roofQuantities.totalSurfaceArea(), totalSurfaceArea);
	setMultiLayeredMass(result, pRoof->material(), volumeCollection, mass);

  return result;
}