/*
** Copyright 2011-2014 Merethis
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

#include "com/centreon/engine/error.hh"
#include "com/centreon/engine/globals.hh"
#include "test/macros/minimal_setup.hh"

using namespace com::centreon::engine;

/**
 *  Setup a minimal Centreon Engine setup.
 */
void com::centreon::engine::test::minimal_setup() {
    // Interval length is 1 second.
  config->interval_length(1);

  // Add host.
  host* hst(add_host(
    const_cast<char*>("myhost"),    // Name
    const_cast<char*>("MyHost"),    // Display name
    const_cast<char*>("MyAlias"),   // Alias
    const_cast<char*>("127.0.0.1"), // Address
    NULL,                           // Check period
    0,                              // Initial state
    1.0,                            // Check interval
    1.0,                            // Retry interval
    1,                              // Max attempts
    0,                              // Host check timeout
    0,                              // Notify up
    1,                              // Notify down
    1,                              // Notify unreachable
    0,                              // Notify flapping
    0,                              // Notify downtime
    1.0,                            // Notification interval
    0,                              // First notification delay
    NULL,                           // Notification period
    1,                              // Notifications enabled
    const_cast<char*>("mycommand"), // Check command
    1,                              // Checks enabled
    1,                              // Accept passive checks
    NULL,                           // Event handler
    0,                              // Event handler enabled
    0,                              // Flap detection enabled
    0.0,                            // Low flap threshold
    0.0,                            // High flap threshold
    0,                              // Flap detection on up
    0,                              // Flap detection on down
    0,                              // Flap detection on unreachable
    0,                              // Stalk on ok
    0,                              // Stalk on down
    0,                              // Stalk on unreachable
    0,                              // Failure prediction enabled
    NULL,                           // Failure prediction options
    0,                              // Check freshness
    0,                              // Freshness threshold
    0,                              // Should be drawn
    0,                              // Retain status information
    0,                              // Retain non-status information
    0,                              // Obsess over
    NULL));                         // System timezone
  if (!hst)
    throw (error() << "default host creation failed");
  host_list->has_been_checked = 1;
  host_list->last_check = time(NULL);
  config->cached_host_check_horizon(24 * 60 * 60);

  // Add service.
  service* svc(add_service(
    const_cast<char*>("myhost"),    // Host name
    const_cast<char*>("myservice"), // Service description
    const_cast<char*>("MyService"), // Display name
    NULL,                           // Check period
    0,                              // Initial state
    1,                              // Max attempts
    0,                              // Service check timeout
    1,                              // Parallelize
    1,                              // Accept passive checks
    1.0,                            // Check interval
    1.0,                            // Retry interval
    1.0,                            // Notification interval
    0,                              // First notification delay
    NULL,                           // Notification period
    0,                              // Notify recovery
    1,                              // Notify unknown
    1,                              // Notify warning
    1,                              // Notify critical
    0,                              // Notify flapping
    0,                              // Notify downtime
    1,                              // Notifications enabled
    0,                              // Volatile
    NULL,                           // Event handler
    0,                              // Event handler enabled
    const_cast<char*>("mycommand"), // Check command
    1,                              // Checks enabled
    0,                              // Flap detection enabled
    0.0,                            // Low flap threshold
    0.0,                            // High flap threshold
    0,                              // Flap detection on ok
    0,                              // Flap detection on warning
    0,                              // Flap detection on unknown
    0,                              // Flap detection on critical
    0,                              // Stalk on ok
    0,                              // Stalk on warning
    0,                              // Stalk on unknown
    0,                              // Stalk on critical
    0,                              // Failure prediction enabled
    NULL,                           // Failure prediction options
    0,                              // Check freshness
    0,                              // Freshness threshold
    0,                              // Retain status information
    0,                              // Retain non-status information
    0,                              // Obsess over service
    NULL));                         // System timezone
  if (!svc)
    throw (error() << "default service creation failed");
  service_list->host_ptr = host_list;
  if (!add_service_link_to_host(hst, svc))
    throw (error() << "could not link default service with default host");

  // Add contact.
  contact* cntct(add_contact(
    const_cast<char*>("mycontact"),       // Name
    const_cast<char*>("MyContact"),       // Alias
    const_cast<char*>("my@contact.info"), // Email
    NULL,                                 // Pager
    NULL,                                 // Addresses
    NULL,                                 // Service notification period
    NULL,                                 // Host notification period
    0,                                    // Notify service ok
    1,                                    // Notify service critical
    1,                                    // Notify service warning
    1,                                    // Notify service unknown
    0,                                    // Notify service flapping
    0,                                    // Notify service downtime
    0,                                    // Notify host up
    1,                                    // Notify host down
    1,                                    // Notify host unreachable
    0,                                    // Notify host flapping
    0,                                    // Notify host downtime
    1,                                    // Host notifications enabled
    1,                                    // Service notifications enabled
    0,                                    // Retain status information
    0,                                    // Retain non-status information
    NULL));                               // System timezone
  if (!cntct)
    throw (error() << "default contact creation failed");
  if (!add_contact_to_host(hst,
         const_cast<char*>("mycontact")))
    throw (error() << "could not add default contact to default host");
  hst->contacts->contact_ptr = cntct;
  if (!add_contact_to_service(svc,
         const_cast<char*>("mycontact")))
    throw (error() << "could not add default contact to default service");
  svc->contacts->contact_ptr = cntct;

  // Add command.
  command* cmd(add_command(
    const_cast<char*>("mycommand"),
    const_cast<char*>("true")));
  if (!cmd)
    throw (error() << "default command creation failed");
  if (!add_host_notification_command_to_contact(
         cntct,
         const_cast<char*>("mycommand")))
    throw (error() << "could not add default host notification command to default contact");
  cntct->host_notification_commands->command_ptr = cmd;
  if (!add_service_notification_command_to_contact(
         cntct,
         const_cast<char*>("mycommand")))
    throw (error() << "could not add default service notification command to default contact");
  cntct->service_notification_commands->command_ptr = cmd;

  return ;
}
