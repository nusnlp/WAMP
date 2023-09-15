Drupal.behaviors.apiExpandable = function() {
  $('div.api-expandable:not(apiExpandableProcessed)').each(function() {
    $(this).addClass('apiExpandableProcessed')
      .children('div.prompt').find('a.show-content').click(function() {
        $(this).parents('div.api-expandable').eq(0)
          .children('.prompt').hide().end()
          .children('.content').show();
        return false;
      }).end().end()
      .children('div.content').find('a.hide-content').click(function() {
        $(this).parents('div.api-expandable').eq(0)
          .children('.prompt').show().end()
          .children('.content').hide();
        return false;
      });
  });
};
