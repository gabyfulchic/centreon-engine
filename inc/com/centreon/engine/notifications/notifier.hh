/*
** Copyright 2017-2018 Centreon
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCE_NOTIFICATIONS_NOTIFIER_HH
#  define CCE_NOTIFICATIONS_NOTIFIER_HH

#  include <ctime>
#  include <list>
#  include "com/centreon/engine/checks/checkable.hh"
#  include "com/centreon/engine/globals.hh"

CCE_BEGIN()

// Forward declaration
class contact;

namespace                  notifications {

  /**
   *  @class notifier notifier.hh "com/centreon/engine/notifications/notifier.hh"
   *  @brief Object validating notifications and sending them if needed.
   *
   */
  class                    notifier : public checks::checkable {
   public:
    typedef bool (notifier::* notifier_filter)();
    typedef void (notifier::* macro_builder)(struct nagios_macros&);
    enum                   acknowledgement_type {
      ACKNOWLEDGEMENT_NONE,
      ACKNOWLEDGEMENT_NORMAL,
      ACKNOWLEDGEMENT_STICKY
    };
    enum                   action_on {
      ON_NONE = 0,
      ON_RECOVERY = (1 << 0),
      ON_UP = (1 << 0),
      ON_OK = (1 << 0),
      ON_DOWN = (1 << 1),
      ON_WARNING = (1 << 1),
      ON_CRITICAL = (1 << 2),
      ON_UNREACHABLE = (1 << 2),
      ON_UNKNOWN = (1 << 3),
      ON_FLAPPING = (1 << 4),
      ON_DOWNTIME = (1 << 5),
    };
    enum                   downtime_propagation {
      DOWNTIME_PROPAGATE_NONE,
      DOWNTIME_PROPAGATE_SIMPLE,
      DOWNTIME_PROPAGATE_TRIGGERED
    };
    enum                   notification_type {
      PROBLEM              = 0,
      RECOVERY             = 1,
      ACKNOWLEDGEMENT      = 2,
      FLAPPINGSTART        = 3,
      FLAPPINGSTOP         = 4,
      FLAPPINGDISABLED     = 5,
      DOWNTIMESTART        = 6,
      DOWNTIMESTOP         = 7,
      DOWNTIMECANCELLED    = 8,
      CUSTOM               = 9,
    };
    enum                   notifier_type {
      HOST_NOTIFICATION,
      SERVICE_NOTIFICATION
    };

                           notifier();
                           notifier(notifier const& other);
    virtual                ~notifier();
    notifier&              operator=(notifier const& other);

    // Configuration.
    int                    get_first_notification_delay() const;
    void                   set_first_notification_delay(int delay);
    timeperiod*            get_notification_period() const;
    void                   set_notification_period(timeperiod* tperiod);
    long                   get_notification_interval() const;
    void                   set_notification_interval(long interval);
    bool                   get_notifications_enabled() const;
    void                   set_notifications_enabled(bool enable);
    bool                   get_notify_on(action_on state) const;
    void                   set_notify_on(action_on state, bool enable);
    bool                   is_state_notification_enabled(
                             int state) const;
    int                    get_recovery_notification_delay() const;
    void                   set_recovery_notification_delay(int delay);

    // Runtime.
    int                    get_current_notification_id() const;
    void                   set_current_notification_id(int id);
    int                    get_current_notification_number() const;
    void                   set_current_notification_number(int number);
    time_t                 get_last_notification() const;
    void                   set_last_notification(
                             time_t last_notification);
    time_t                 get_next_notification() const;
    void                   set_next_notification(
                             time_t next_notification);
    void                   add_notification_flag(
                             notifier::notification_type type);
    void                   notify(
                             notification_type type,
                             std::string const& author = "",
                             std::string const& comment = "",
                             int options = 0);

    // Contacts / contact groups.
    void                   add_contact(engine::contact* user);
    void                   add_contactgroup(engine::contactgroup* cg);
    void                   clear_contacts();
    void                   clear_contactgroups();
    bool                   contains_contact(
                             std::string const& username) const;
    umap<std::string, engine::contact*> const&
                           get_contacts() const;
    umap<std::string, engine::contactgroup*> const&
                           get_contactgroups() const;

    // Acknowledgement.
    int                    get_acknowledgement_timeout() const;
    void                   set_acknowledgement_timeout(int timeout);
    acknowledgement_type   get_acknowledgement_type() const;
    bool                   is_acknowledged() const;
    void                   set_acknowledged(acknowledgement_type type);
    time_t                 get_last_acknowledgement() const;
    void                   set_last_acknowledgement(
                             time_t last_acknowledgement);
    void                   schedule_acknowledgement_expiration();
    void                   update_acknowledgement_on_state_change();

    // Downtime.
    bool                   is_in_downtime() const;
    void                   set_in_downtime(bool downtime);
    int                    get_pending_flex_downtime() const;
    void                   inc_pending_flex_downtime();
    void                   dec_pending_flex_downtime();
    int                    get_scheduled_downtime_depth() const;
    void                   inc_scheduled_downtime_depth();
    void                   dec_scheduled_downtime_depth();
    int                    schedule_downtime(
                             downtime::downtime_type type,
                             time_t entry_time,
                             std::string const& author,
                             std::string const& comment_data,
                             time_t start_time,
                             time_t end_time,
                             bool fixed,
                             unsigned long triggered_by,
                             unsigned long duration,
                             downtime_propagation propagate =
                               DOWNTIME_PROPAGATE_NONE);

    // XXX
    void                   delete_acknowledgement_comments();
    void                   delete_all_comments();
    unsigned int           get_current_notifications_flag() const;
    bool                   get_no_more_notifications() const;
    void                   check_pending_flex_downtime();
    bool                   should_be_escalated() const;
    bool                   get_recovery_been_sent() const;
    time_t                 get_initial_notif_time() const;
    void                   set_initial_notif_time(time_t initial);
//    void                   set_no_more_notifications(bool no_more);
    void                   set_recovery_been_sent(bool sent);
    std::string            get_info();
    void                   set_flapping_comment_id(unsigned int id);
    unsigned int           get_flapping_comment_id() const;

   protected:
    virtual void           _checkable_macro_builder(
                             nagios_macros& mac) = 0;

   private:
    unsigned int           _current_notifications;
    bool                   _in_downtime;
    notifier_filter        _get_filter(notification_type type) const;
    macro_builder          _get_macro_builder(
                             notification_type type) const;
    bool                   _problem_filter();
    bool                   _recovery_filter();
    bool                   _acknowledgement_filter();
    bool                   _flappingstart_filter();
    bool                   _flappingstopdisabled_filter();
    bool                   _downtimestart_filter();
    bool                   _downtimestopcancelled_filter();
    bool                   _custom_filter();
    void                   _internal_copy(notifier const& other);

    void                   _problem_macro_builder(nagios_macros& mac);
    void                   _recovery_macro_builder(nagios_macros& mac);
    bool                   _notification_is_active(
                             notification_type type) const;

    static notifier_filter const
                           _filter[];
    static macro_builder   _macro_builder[];
    static std::string const
                           _notification_string[];

    int                    _acknowledgement_timeout;
    umap<std::string, engine::contact*>
                           _contacts;
    umap<std::string, engine::contactgroup*>
                           _contact_groups;
    acknowledgement_type   _current_acknowledgement;
    int                    _current_notification_id;
    int                    _current_notification_number;
    bool                   _escalate_notification;
    int                    _first_notification_delay;
    time_t                 _last_acknowledgement;
    time_t                 _last_notification;
    time_t                 _next_notification;
    long                   _notification_interval;
    timeperiod*            _notification_period;
    bool                   _notifications_enabled;
    int                    _notified_states;
    int                    _pending_flex_downtime;
    bool                   _recovery_been_sent;
    int                    _recovery_notification_delay;
    int                    _scheduled_downtime_depth;
    unsigned int           _flapping_comment_id;
  };
}

CCE_END()

#endif // !CCE_NOTIFICATIONS_NOTIFIER_HH
