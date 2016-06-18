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
 * This ItemAddOption uses item file name to set origination date
 *
 */
class DateFromFileNameOption extends ItemAddOption {

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
      $errors = $warnings = array();
      /* Copy the array because we will change it with do / while / array_splice */
      $itemsInBatches = $items;
      /*
       * Batch size should be <= ulimit max open files, as long as we don't query this value,
       * assume a value of 100 which is fairly low
       */
      $batchSize = 100;
      do {
         $currentItems = array_splice($itemsInBatches, 0, $batchSize);
         $itemIds = array();
         foreach ($currentItems as $item) {
            $itemIds[] = $item->getId();
         }
         list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($itemIds);
         if ($ret) {
            return array($ret, null, null);
         }

         for ($i = 0; $i < count($currentItems); $i++) {
            /* Only look at GalleryDataItems */
            if (!GalleryUtilities::isA($currentItems[$i], 'GalleryDataItem')) {
               continue;
            }

            list ($ret, $currentItems[$i]) = $currentItems[$i]->refresh();
            if ($ret) {
               GalleryCoreApi::releaseLocks($lockId);
               return array($ret, null, null);
            }

            list ($ret, $path) = $currentItems[$i]->fetchPath();
            if ($ret) {
               GalleryCoreApi::releaseLocks($lockId);
               return array($ret, null, null);
            }

            $baseFileName = basename($path);

            /* see if date and time is specified. If failed, see if date
             * is specified. If failed, just continue */
            $hour = $minute = $second = 0;
            if (ereg ("(^|.*[^0-9])([0-9]{4})-?([0-9]{2})-?([0-9]{2})[_T -]([0-9]{2})[-:]?([0-9]{2})[-:]?([0-9]{2})([^0-9].*|$)", $baseFileName, $regs)) {
               $second = (int)($regs[7]);
               $minute = (int)($regs[6]);
               $hour = (int)($regs[5]);
               $day = (int)($regs[4]);
               $month = (int)($regs[3]);
               $year = (int)($regs[2]);
            } elseif (ereg ("(^|.*[^0-9])([0-9]{4})-?([0-9]{2})-?([0-9]{2})[_T -]([0-9]{2})[-:]?([0-9]{2})([^0-9].*|$)", $baseFileName, $regs)) {
               $minute = (int)($regs[6]);
               $hour = (int)($regs[5]);
               $day = (int)($regs[4]);
               $month = (int)($regs[3]);
               $year = (int)($regs[2]);
            } elseif (ereg ("(^|.*[^0-9])([0-9]{4})-?([0-9]{2})-?([0-9]{2})([^0-9].*|$)", $baseFileName, $regs)) {
               $day = (int)($regs[4]);
               $month = (int)($regs[3]);
               $year = (int)($regs[2]);
            }
            else {
               continue;
            }

            /* validate timestamp */
            if (!checkdate($month, $day, $year))
               continue;
            if ($year > date('Y'))
               continue;
            if ($year < 1900)
               continue;
            $originationTimestamp = mktime($hour, $minute, $second, $month, $day, $year);
            if (($originationTimestamp == false) || ($originationTimestamp == -1))
               continue;

            $currentItems[$i]->setOriginationTimestamp($originationTimestamp);
            $ret = $currentItems[$i]->save();
            if ($ret) {
               GalleryCoreApi::releaseLocks($lockId);
               return array($ret, null, null);
            }
         }

         $ret = GalleryCoreApi::releaseLocks($lockId);
         if ($ret) {
            return array($ret, null, null);
         }
      } while (!empty($itemsInBatches));

      return array(null, $errors, $warnings);
   }
}
?>
