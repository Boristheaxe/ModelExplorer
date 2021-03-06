//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#include "stdafx.h"
#include "ModelExplorerPlugin.h"
#include "PluginToolButtons.h"
#include "RengaEventsHandler.h"
#include "ModelExplorerWidget.h"

#include <RengaAPI/Localization.h>
#include <RengaAPI/Message.h>

static const QString russianLocaleFileName = "modelexplorerplugin_ru.qm";
static const QString englishLocaleFileName = "modelexplorerplugin_en.qm";
static const QString defaultLocaleFileName = englishLocaleFileName;

static const QString c_russianLocale = "ru_RU";
static const QString c_englishLocale = "en_EN";

static const rengabase::String c_loadLocalizationFileError = L"Cannot open localization file.";
static const rengabase::String c_error = L"Error";

ModelExplorerPlugin::ModelExplorerPlugin()
{
#ifdef _DEBUG
//   NOTE: To debug a plugin you should create a debug QApplication in the plugin, 
//   put the debug Qt dlls (can be found in separate repository) into the plugin folder (Renga installation folder/Plugins/PluginName), 
//   put qwindowsd.dll into the "platform" folder in the plugin folder
//   and set library paths to the application, as shown below
  
  QStringList paths = QCoreApplication::libraryPaths();
  paths.append("your plugin folder path");
  QCoreApplication::setLibraryPaths(paths);
  
  int argc = 0;
  char** argv = 0;
  m_pApp.reset(new QApplication(argc, argv));
#endif
}

ModelExplorerPlugin::~ModelExplorerPlugin()
{}

bool ModelExplorerPlugin::initialize(const wchar_t* pluginPath)
{
	if(!loadTranslator(pluginPath))
    return false;

  subscribeOnRengaEvents();
  addPluginButtons(pluginPath);

  return true;
}

void ModelExplorerPlugin::stop()
{
  m_pPluginToolButtons.reset(nullptr);
	m_pRengaEventsHandler.reset(nullptr);
  m_pWidget.reset(nullptr);
}

QString ModelExplorerPlugin::translationFileName()
{
  // get application locale
  QString appLocale = rengaStringToQString(rengaapi::Localization::currentLocale_());

  // get translation file name
  if (appLocale == c_russianLocale)
    return russianLocaleFileName;
  else if(appLocale == c_englishLocale)
    return englishLocaleFileName;
  else
    return defaultLocaleFileName;
}

bool ModelExplorerPlugin::loadTranslator(const std::wstring& pluginPath)
{
  // load QTranslator
  if (!m_translator.load(translationFileName(), QString::fromStdWString(pluginPath)))
  {
    rengaapi::Message::showMessageBox(rengaapi::Message::Error, c_error, c_loadLocalizationFileError);
    return false;
  }
  else
  {
    return true;
  }
}

void ModelExplorerPlugin::subscribeOnRengaEvents()
{
  m_pRengaEventsHandler.reset(new RengaEventsHandler());
  connect(m_pRengaEventsHandler.get(), SIGNAL(projectAboutToClose()), this, SLOT(onProjectAboutToClose()));
}

void ModelExplorerPlugin::addPluginButtons(const std::wstring& pluginPath)
{
  m_pPluginToolButtons.reset(new PluginToolButtons(pluginPath, &m_translator));
  connect(m_pPluginToolButtons.get(), SIGNAL(modelExplorerButtonClicked()), this, SLOT(onModelExplorerButtonClicked()));
}

void ModelExplorerPlugin::onModelExplorerButtonClicked()
{
  m_pWidget.reset(new ModelExplorerWidget(&m_translator));
  m_pWidget->readModelAndShow();
}

void ModelExplorerPlugin::onProjectAboutToClose()
{
  if(m_pWidget)
  {
    m_pWidget->close();
    m_pWidget.reset(nullptr);
  }
}
