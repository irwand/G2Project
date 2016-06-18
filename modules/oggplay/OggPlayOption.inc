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
 * This ItemAddOption adds the OggPlayRenderer for any audio and video ogg item.
 * @package OggPlay
 * @subpackage UserInterface
 * @author Marco Nelles <ogg@maniatek.de>
 * @version $Revision: 1 $
 */
class OggPlayOption extends ItemAddOption {

    /**
     * @see ItemAddOption::isAppropriate
     */
    function isAppropriate() {
	return array(null, true);
    }

    /**
     * @see ItemAddOption::handleRequestAfterAdd
     */
    function handleRequestAfterAdd($form, $items) {
	global $gallery;
	$storage =& $gallery->getStorage();
	$errors = $oggItems = array();

	foreach ($items as $item) {
	    if (GalleryUtilities::isA($item, 'GalleryDataItem')
		    && ($item->getMimeType() == 'audio/ogg' || $item->getMimeType() == 'video/ogg')) {
		$oggItems[] = $item;
	    }
	}
	while (!empty($oggItems)) {
	    $currentItems = array_splice($oggItems, 0, 100);
	    $itemIds = array();
	    foreach ($currentItems as $item) {
		$itemIds[] = $item->getId();
	    }

	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($itemIds);
	    if ($ret) {
		return array($ret, null, null);
	    }

	    foreach ($currentItems as $item) {
		list ($ret, $item) = $item->refresh();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array($ret, null, null);
		}

		$item->setRenderer('OggPlayRenderer');
		$ret = $item->save();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array($ret, null, null);
		}
	    }

	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $ret = $storage->checkPoint();
	    if ($ret) {
		return array($ret, null, null);
	    }
	}

	return array(null, $errors, array());
    }
}
?>