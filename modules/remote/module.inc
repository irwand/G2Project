<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2008 Bharat Mediratta
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @package Remote
 * @author Pierre-Luc Paour <paour@users.sourceforge.net>
 * @version $Revision: 20951 $
 */
class RemoteModule extends GalleryModule {

    function RemoteModule() {
	global $gallery;
	$this->setId('remote');
	$this->setName($gallery->i18n('Remote'));
	$this->setDescription($gallery->i18n('Implementation for the remote control protocol'));
	/** 
	 * Version = MajorModuleAPI.MinorModuleAPI.SerialNumberMajor.SerialNumberMinor
	 * Update upgrade() function below too
	**/
	$this->setVersion('4.0.0.0');
	$this->setRequiredModuleApi(array(4, 0));
	$this->setRequiredCoreApi(array(8, 0));
	$this->setTemplateVersion(1);
	$this->setGroup('interfaces', $gallery->i18n('Remote Interfaces'));
	$this->setCallbacks('getSiteAdminViews');
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'remote');
	if ($ret) {
	    return $ret;
	}
	foreach (array('showItemAdd' => 1) as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	/* Register our item add plugins */
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddPlugin', 'ItemAddGalleryRemote', 'ItemAddGalleryRemote',
	    'modules/remote/ItemAddGalleryRemote.inc', 'remote', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Remote'),
				 'view' => 'remote.RemoteSiteAdmin')));
    }
}
?>
