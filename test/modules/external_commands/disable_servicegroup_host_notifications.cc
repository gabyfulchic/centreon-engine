/*
** Copyright 2011-2015 Merethis
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

#include <exception>
#include "com/centreon/engine/error.hh"
#include "com/centreon/engine/modules/external_commands/commands.hh"
#include "com/centreon/engine/globals.hh"
#include "com/centreon/logging/engine.hh"
#include "test/unittest.hh"

using namespace com::centreon::engine;

/**
 *  Run disable_servicegroup_host_notifications test.
 */
static int check_disable_servicegroup_host_notifications(int argc, char** argv) {
  (void)argc;
  (void)argv;

  host* hst = unittest::add_generic_host();
  if (!hst)
    throw (engine_error() << "create host failed.");

  service* svc = unittest::add_generic_service();
  if (!svc)
    throw (engine_error() << "create service failed.");

  servicegroup* group = add_servicegroup("group", NULL);
  if (!group)
    throw (engine_error() << "create servicegroup failed.");

  servicesmember* smember = add_service_to_servicegroup(group, "name", "description");
  if (!smember)
    throw (engine_error() << "create servicesmember failed.");
  smember->service_ptr = svc;

  hst->notifications_enabled = true;
  char const* cmd("[1317196300] DISABLE_SERVICEGROUP_HOST_NOTIFICATIONS;group");
  process_external_command(cmd);

  if (hst->notifications_enabled)
    throw (engine_error() << "disable_servicegroup_host_notifications failed.");
  return (0);
}

/**
 *  Init unit test.
 */
int main(int argc, char** argv) {
  unittest utest(argc, argv, &check_disable_servicegroup_host_notifications);
  return (utest.run());
}
