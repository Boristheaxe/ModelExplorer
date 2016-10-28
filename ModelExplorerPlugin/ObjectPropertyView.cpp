//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#include "ObjectPropertyView.h"
#include "ObjectPropertyViewBuilderFactory.h"
#include "RengaStringToQString.h"

#include <memory>

#include <RengaAPI/Project.h>


ObjectPropertyView::ObjectPropertyView(QTranslator* pTranslator, QWidget* pParent)
  : QtTreePropertyBrowser(pTranslator, pParent)
  , m_pTranslator(pTranslator)
  , m_currentObjectId(0)
  , m_propertyViewMode(CategoryMode)
{
  m_pGroupManager = new QtGroupPropertyManager(this);
  initPropertyManagers();
}

void ObjectPropertyView::setSelectedObjectId(const rengaapi::ObjectId objId)
{
	m_currentObjectId = objId;
	buildPropertyView();
}

void ObjectPropertyView::changeMode(ObjectPropertyView::Mode newMode)
{
	m_propertyViewMode = newMode;
	buildPropertyView();
}

void ObjectPropertyView::initPropertyManagers()
{
  m_propertyManagers.m_pIntManager = new QtIntPropertyManager(this);
  m_propertyManagers.m_pDoubleManager = new QtStringPropertyManager(this, QtStringPropertyManager::valueTupe::doubleType);
  m_propertyManagers.m_pDoubleUserAttributeManager = new QtStringPropertyManager(this, QtStringPropertyManager::valueTupe::doubleType);
  m_propertyManagers.m_pStringManager = new QtStringPropertyManager(this, QtStringPropertyManager::valueTupe::stringType);
  m_propertyManagers.m_pStringUserAttributeManager = new QtStringPropertyManager(this, QtStringPropertyManager::valueTupe::stringType);

  auto pIntReadOnlyPropertyFactory = new QtSpinBoxFactory(this, true);
  auto pDoublePropertyFactory = new QtLineEditFactory(this, QtLineEditFactory::valueType::doubleType, false);
  auto pDoubleReadOnlyPropertyFactory = new QtLineEditFactory(this, QtLineEditFactory::valueType::doubleType, true);
  auto pStringPropertyFactory = new QtLineEditFactory(this, QtLineEditFactory::valueType::stringType, false);
  auto pStringReadOnlyPropertyFactory = new QtLineEditFactory(this, QtLineEditFactory::valueType::stringType, true);

  setFactoryForManager(m_propertyManagers.m_pIntManager, pIntReadOnlyPropertyFactory);
  setFactoryForManager(m_propertyManagers.m_pDoubleManager, pDoubleReadOnlyPropertyFactory);
  setFactoryForManager(m_propertyManagers.m_pDoubleUserAttributeManager, pDoublePropertyFactory);
  setFactoryForManager(m_propertyManagers.m_pStringManager, pStringReadOnlyPropertyFactory);
  setFactoryForManager(m_propertyManagers.m_pStringUserAttributeManager, pStringPropertyFactory);

   // handle user attributes changes
   QObject::connect(
     m_propertyManagers.m_pDoubleUserAttributeManager, SIGNAL(valueChanged(QtProperty*, const QString&)),
     this, SLOT(userDoubleAttributeChanged(QtProperty*, const QString&)));
   QObject::connect(
     m_propertyManagers.m_pStringUserAttributeManager, SIGNAL(valueChanged(QtProperty*, const QString&)),
     this, SLOT(userStringAttributeChanged(QtProperty*, const QString&)));
}

void ObjectPropertyView::clearPropertyManagers()
{
  m_propertyManagers.m_pIntManager->clear();
  m_propertyManagers.m_pDoubleManager->clear();
  m_propertyManagers.m_pDoubleUserAttributeManager->clear();
  m_propertyManagers.m_pStringManager->clear();
  m_propertyManagers.m_pStringUserAttributeManager->clear();
  m_pGroupManager->clear();
}

void ObjectPropertyView::buildPropertyViewAsList(const PropertyMap& parameters, const PropertyMap& calculated, const PropertyMap& userDefinedProperties)
{
  // show in list mode
  PropertyMap allProperties;
  allProperties.insert(parameters.begin(), parameters.end());
  allProperties.insert(calculated.begin(), calculated.end());
  allProperties.insert(userDefinedProperties.begin(), userDefinedProperties.end());

  for (auto it = allProperties.begin(); it != allProperties.end(); ++it)
    addProperty(it->second);
}

void ObjectPropertyView::buildPropertyViewSingleCategory(const QString& categoryName, const PropertyMap& categoryProperties)
{
  if(categoryProperties.empty())
    return;

  QtProperty* pCategoryProperty = m_pGroupManager->addProperty(categoryName);
  pCategoryProperty->setBold(true);

  for (auto it = categoryProperties.begin(); it != categoryProperties.end(); ++it)
    pCategoryProperty->addSubProperty(it->second);

  addProperty(pCategoryProperty);
}

void ObjectPropertyView::buildPropertyViewByCategory(const PropertyMap& parameters, const PropertyMap& calculated, const PropertyMap& userDefinedProperties)
{
  buildPropertyViewSingleCategory(m_pTranslator->translate("propertyView", "Object parameters"), parameters);
  buildPropertyViewSingleCategory(m_pTranslator->translate("propertyView", "Calculated characteristics"), calculated);
  buildPropertyViewSingleCategory(m_pTranslator->translate("propertyView", "Object properties"), userDefinedProperties);
}

bool ObjectPropertyView::createProperties(PropertyMap& parameters, PropertyMap& calculated, PropertyMap& userDefinedProperties)
{
  // get modelObject from project
  rengaapi::Model projectModel = rengaapi::Project::model();
  rengaapi::ModelObjectCollection pModelObjects = projectModel.objects();
  rengaapi::ModelObject* pSelectedObject = pModelObjects.get(m_currentObjectId);

  // check if object was found in collection
  if (pSelectedObject == nullptr)
    return false;

  ObjectPropertyViewBuilderFactory propertyBuilderFactory(&m_propertyManagers, m_pTranslator);
  std::unique_ptr<IObjectPropertyViewBuilder> objectPropertyViewBuilder(propertyBuilderFactory.createBuilder(pSelectedObject));

  parameters = objectPropertyViewBuilder->createParametersProperties(pSelectedObject);
  calculated = objectPropertyViewBuilder->createQuantitiesProperties(pSelectedObject);
  userDefinedProperties = objectPropertyViewBuilder->createUserAttributesProperties(pSelectedObject);
  return true;
}

void ObjectPropertyView::buildPropertyView()
{	
  clearPropertyManagers();

  PropertyMap parameters, calculated, userDefinedProperties;
  if(createProperties(parameters, calculated, userDefinedProperties) == false)
    return;

  if (m_propertyViewMode == Mode::ListMode)
    buildPropertyViewAsList(parameters, calculated, userDefinedProperties);
  else
    buildPropertyViewByCategory(parameters, calculated, userDefinedProperties);
}

void ObjectPropertyView::userDoubleAttributeChanged(QtProperty* property, const QString& newValue)
{
  bool ok = false;
  double newDoubleValue = newValue.toDouble(&ok);
  if (ok)
  {
    rengaapi::UserAttributeValue userAttributeValue = rengaapi::UserAttributeValue(newDoubleValue);
    changeUserAttribute(property, userAttributeValue);
  }
}

void ObjectPropertyView::userStringAttributeChanged(QtProperty* property, const QString& newValue)
{
  rengaapi::UserAttributeValue userAttributeValue = rengaapi::UserAttributeValue(rengabase::rengaStringFromStdWString(newValue.toStdWString()));
  changeUserAttribute(property, userAttributeValue);
}

void ObjectPropertyView::changeUserAttribute(QtProperty* property, rengaapi::UserAttributeValue userAttributeValue)
{
  rengaapi::Model projectModel = rengaapi::Project::model();
  rengaapi::ModelObjectCollection modelObjectCollection = projectModel.objects();
  rengaapi::ModelObject* pSelectedObject = modelObjectCollection.get(rengaapi::ObjectId(m_currentObjectId));

  if(pSelectedObject == nullptr)
    return;

  rengabase::String userAttributeUuidAsString = rengabase::rengaStringFromStdWString(property->data().toStdWString());
  rengabase::UUID userAttributeIdUuid = rengabase::UUID::fromString(userAttributeUuidAsString);
  rengaapi::UserAttributeId userAttributeId = rengaapi::UserAttributeId(userAttributeIdUuid);

  rengaapi::UserAttributeRegister userAttributeRegister = rengaapi::Project::userAttributeRegister();
  if (userAttributeRegister.typeHasAttribute(pSelectedObject->type(), userAttributeId))
  {
    rengaapi::Operation modelOperation = projectModel.createOperation();

    // start model operation
    if (modelOperation.start().isOk())
    {
      // change attribute value
      pSelectedObject->setUserAttributeValue(userAttributeId, userAttributeValue);
      modelOperation.apply();
      return;
    }
  }
}