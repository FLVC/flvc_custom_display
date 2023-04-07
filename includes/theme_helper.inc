<?php

/**
 * @file
 * A collection of helper functions that process data for display
 */

@include_once 'sites/all/libraries/htmlpurifier/library/HTMLPurifier.auto.php';


// Parses the RELATED-LINKS XML file and turns it into HTML
function create_related_links_html($islandora_object) {
  libxml_use_internal_errors(true);
  $related_links_html = '<div id="related-links-section">';
  $related_links_xml_object = simplexml_load_string($islandora_object['RELATED-LINKS']->content);
  
  if ($related_links_xml_object === false) {
    echo "Failed loading XML<br /><br />Error: ";
    foreach(libxml_get_errors() as $error) {
        echo "\t", $error->message;
    }
  }
  else {

    if (isset($related_links_xml_object['title'])) {
      $related_links_html .= '<h2>' . $related_links_xml_object['title'] . '</h2>';
    }

    foreach ($related_links_xml_object->children() as $xml_child) {

      if ($xml_child->getName() == 'external-link') {
        $related_links_html .= '<li><a href="' . $xml_child . '" target="_blank">';

        if (isset($xml_child['title'])) {
          $related_links_html .= $xml_child['title'] . '</a></li>';
        }
        else {
          $related_links_html .= $xml_child . '</a></li>';
        }
      }

      if ($xml_child->getName() == 'internal-link') {
        $related_links_html .= '<li><a href="' . $xml_child . '">';

        if (isset($xml_child['title'])) {
          $related_links_html .= $xml_child['title'] . '</a></li>';
        }
        else {
          $related_links_html .= $xml_child . '</a></li>';
        }
      }

      if ($xml_child->getName() == 'page') {
        $related_links_html .= '<li><a href="#' . $xml_child . '">';

        if (isset($xml_child['title'])) {
          $related_links_html .= $xml_child['title'] . '</a></li>';
        }
        else {
          $related_links_html .= $xml_child . '</a></li>';
        }

        $related_links_html .= '<div id="' . $xml_child . '" class="modalCSSPage">';
        
        //retrieve page html and strip any unsafe tags
        if (isset($islandora_object[(string) $xml_child])) {
          $config = HTMLPurifier_Config::createDefault();
          $config->set('Attr.EnableID', true);
          $purifier = new HTMLPurifier($config);
          $clean_html = $purifier->purify($islandora_object[(string) $xml_child]->content);
          $final_html = addAccordionFeature($clean_html);
          $related_links_html .= '<div>' . '<a href="#close" title="Close" class="closeModal">X</a>';
          $related_links_html .= $final_html . '</div>';
        }

        $related_links_html .= '</div>';
      }
    }
  }

  $related_links_html .= '</div>';
  
  return $related_links_html;
}

// check for the presence of an accordion div
function addAccordionFeature($clean_html) {
  $final_html = $clean_html;
  $dom = new DOMDocument();
  $dom->loadHtml($clean_html);
  foreach ($dom->getElementsByTagName('div') as $div) {
    if ($div->getAttribute('class') == 'accordion') {
      $final_html = $clean_html . '<script>jQuery( "#' . $div->getAttribute('id') . 
                     '" ).accordion({collapsible: true, active: false, autoHeight: false});</script>';
    }
  } 
  
  return $final_html;
}
