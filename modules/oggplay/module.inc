<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2009 Bharat Mediratta
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
 * The implementation of the OggPlay module
 * @package OggPlay
 * @author Marco Nelles <ogg@maniatek.de>
 * @version $Revision: 1 $
 */
class OggPlayModule extends GalleryModule {

    function OggPlayModule() {
	global $gallery;
	$this->setId('oggplay');
	$this->setName($gallery->i18n('OggPlay'));
	$this->setDescription(
		$gallery->i18n('Enable inline play of Ogg Vorbis and Ogg Theora/Vorbis files with html 5 video and audio tags'));
	$this->setVersion('0.0.1');
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryRenderer', 'OggPlayRenderer', 'OggPlayRenderer',
	    'modules/oggplay/classes/OggPlayRenderer.class', 'oggplay', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddOption', 'OggPlayOption', 'OggPlayOption',
	    'modules/oggplay/OggPlayOption.inc', 'oggplay', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	global $gallery;
	$storage =& $gallery->getStorage();

	/* Add renderer on all GalleryDataItems with ogg/audio or ogg/video mime type that need it */
	$query = "SELECT [GalleryDataItem::id] FROM [GalleryDataItem]
		  WHERE [GalleryDataItem::mimeType] = 'video/ogg' OR [GalleryDataItem::mimeType] = 'audio/ogg'";
	list ($ret, $searchResults) = $gallery->search($query, array(), array(), 1);
	if ($ret) {
	    return array($ret, null);
	}
	$ids = array();
	while ($result = $searchResults->nextResult()) {
	    $ids[] = (int)$result[0];
	}
	while (!empty($ids)) {
	    $itemIds = array_splice($ids, 0, 100);
	    list ($ret, $items) = GalleryCoreApi::loadEntitiesById($itemIds, 'GalleryDataItem');
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach ($items as $item) {
		if (GalleryUtilities::isA($item, 'GalleryDataItem') && !$item->getRenderer()) {
		    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($item->getId());
		    if ($ret) {
			return array($ret, null);
		    }
		    list ($ret, $item) = $item->refresh();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null);
		    }
		    $item->setRenderer('OggPlayRenderer');
		    $ret = $item->save();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null);
		    }
		    $ret = GalleryCoreApi::releaseLocks($lockId);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }
	    $ret = $storage->checkPoint();
	    if ($ret) {
		return array($ret, null);
	    }
	}

	list ($ret, $redirect) = parent::activate($postActivationEvent);
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $redirect);
    }

    /**
     * @see GalleryPlugin::uninstall
     */
    function uninstall() {
	$ret = GalleryCoreApi::deleteRenderer('OggPlayRenderer');
	if ($ret) {
	    return $ret;
	}
	return parent::uninstall();
    }
}
?>
