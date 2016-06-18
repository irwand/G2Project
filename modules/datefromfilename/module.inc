<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2007 Bharat Mediratta
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
 * @package DateFromFileName
 * @version $Revision: 15513 $
 * @author Irwan Djajadi
 */

/**
 * DateFromFileName
 *
 * This is a simple module to get origination date from item file name
 *
 * @package Datefromfilename
 */
class DatefromfilenameModule extends GalleryModule {

   function DatefromfilenameModule() {
      global $gallery;
      $this->setId('datefromfilename');
      $this->setName($gallery->i18n('DateFromFileName'));
      $this->setDescription($gallery->i18n('Get origination date from file name'));
      $this->setVersion('1.0.0');
      $this->setCallbacks('registerEventListeners');
      $this->setGroup('other', $gallery->i18n('Other'));
      $this->setRequiredCoreApi(array(7, 0));
      $this->setRequiredModuleApi(array(3, 0));
   }

   /**
    * @see GalleryModule::performFactoryRegistrations
    */
   function performFactoryRegistrations() {
      $ret = GalleryCoreApi::registerFactoryImplementation('ItemAddOption',
         'DateFromFileNameOption', 'DateFromFileNameOption',
         'modules/datefromfilename/DateFromFileNameOption.inc',
         'datefromfilename', null);
      if ($ret) {
         return $ret;
      }

      return null;
   }

}
?>
