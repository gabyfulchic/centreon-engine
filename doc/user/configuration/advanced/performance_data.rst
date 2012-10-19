Performance Data
****************

Introduction
============

Centreon Engine is designed to allow
:ref:`plugins <get_started/plugins>`
to return optional performance data in addition to normal status data,
as well as allow you to pass that performance data to external
applications for processing. A description of the different types of
performance data, as well as information on how to go about processing
that data is described below...

Types of Performance Data
=========================

There are two basic categories of performance data that can be obtained
from Centreon Engine:

  * Check performance data
  * Plugin performance data

Check performance data is internal data that relates to the actual
execution of a host or service check. This might include things like
service check latency (i.e. how "late" was the service check from its
scheduled execution time) and the number of seconds a host or service
check took to execute. This type of performance data is available for
all checks that are performed. The
:ref:`$HOSTEXECUTIONTIME$ <basics_standard_macros#standard_macrosmacroavailabilitycharthostexecutiontime>`
and :ref:`$SERVICEEXECUTIONTIME$ <basics_standard_macros#standard_macrosmacroavailabilitychartserviceexecutiontime>`
:ref:`macros <basics_understanding_macros_and_how_they_work>` can be
used to determine the number of seconds a host or service check was
running and the :ref:`$HOSTLATENCY$ <basics_standard_macros#standard_macrosmacroavailabilitycharthostlatency>`
and :ref:`$SERVICELATENCY$ <basics_standard_macros#standard_macrosmacroavailabilitychartservicelatency>`
macros can be used to determine how "late" a regularly-scheduled host or
service check was.

Plugin performance data is external data specific to the plugin used to
perform the host or service check. Plugin-specific data can include
things like percent packet loss, free disk space, processor load, number
of current users, etc. - basically any type of metric that the plugin is
measuring when it executes. Plugin-specific performance data is optional
and may not be supported by all plugins. Plugin-specific performance
data (if available) can be obtained by using the :ref:`$HOSTPERFDATA$ <basics_standard_macros#standard_macrosmacroavailabilitycharthostperfdata>`
and :ref:`$SERVICEPERFDATA$ <basics_standard_macros#standard_macrosmacroavailabilitychartserviceperfdata>`
:ref:`macros <basics_understanding_macros_and_how_they_work>`. Read on
for more information on how plugins can return performance data to
Centreon Engine for inclusion in the $HOSTPERFDATA$ and
$SERVICEPERFDATA$ macros.

Plugin Performance Data
=======================

At a minimum, Centreon Engine plugins must return a single line of
human-readable text that indicates the status of some type of measurable
data. For example, the check_ping plugin might return a line of text
like the following::

  PING ok - Packet loss = 0%, RTA = 0.80 ms

With this simple type of output, the entire line of text is available in
the $HOSTOUTPUT$ or $SERVICEOUTPUT$
:ref:`macros <basics_understanding_macros_and_how_they_work>`
(depending on whether this plugin was used as a host check or service
check).

Plugins can return optional performance data in their output by sending
the normal, human-readable text string that they usually would, followed
by a pipe character (|), and then a string containing one or more
performance data metrics. Let's take the check_ping plugin as an example
and assume that it has been enhanced to return percent packet loss and
average round trip time as performance data metrics. Sample output from
the plugin might look like this::

  PING ok - Packet loss = 0%, RTA = 0.80 ms | percent_packet_loss=0, rta=0.80

When Centreon Engine sees this plugin output format it will split the
output into two parts:

  * Everything before the pipe character is considered to be the
    "normal" plugin output and will be stored in either the $HOSTOUTPUT$
    or $SERVICEOUTPUT$ macro
  * Everything after the pipe character is considered to be the
    plugin-specific performance data and will be stored in the
    $HOSTPERFDATA$ or $SERVICEPERFDATA$ macro

In the example above, the $HOSTOUTPUT$ or $SERVICEOUTPUT$ macro would
contain "PING ok - Packet loss = 0%, RTA = 0.80 ms" (without quotes) and
the $HOSTPERFDATA$ or $SERVICEPERFDATA$ macro would contain
"percent_packet_loss=0, rta=0.80" (without quotes).

Multiple lines of performace data (as well as normal text output) can be
obtained from plugins, as described in the
:ref:`plugin API documentation <centengine_plugin_api>`.

.. note::

   The Centreon Engine daemon doesn't directly process plugin
   performance data, so it doesn't really care what the performance data
   looks like. There aren't really any inherent limitations on the
   format or content of the performance data. However, if you are using
   an external addon to process the performance data (i.e. PerfParse),
   the addon may be expecting that the plugin returns performance data
   in a specific format. Check the documentation that comes with the
   addon for more information.

Processing Performance Data
===========================

If you want to process the performance data that is available from
Centreon Engine and the plugins, you'll need to do the following:

  * Enable the
    :ref:`process_performance_data <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariablesperformance_dataprocessingoption>`
    option.
  * Configure Centreon Engine so that performance data is either written
    to files and/or processed by executing commands.

Read on for information on how to process performance data by writing to
files or executing commands.

Processing Performance Data Using Commands
==========================================

The most flexible way to process performance data is by having Centreon
Engine execute commands (that you specify) to process or redirect the
data for later processing by external applications. The commands that
Centreon Engine executes to process host and service performance data
are determined by the
:ref:`host_perfdata_command <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariableshostperformance_dataprocessingcommand>`
and :ref:`service_perfdata_command <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariablesserviceperformance_dataprocessingcommand>`
options, respectively.

An example command definition that redirects service check performance
data to a text file for later processing by another application is shown
below::

  define command{
    command_name store-service-perfdata
    command_line /bin/echo -e "$LASTSERVICECHECK$\t$HOSTNAME$\t$SERVICEDESC$\t$SERVICESTATE$\t$SERVICEATTEMPT$\t$SERVICESTATETYPE$\t$SERVICEEXECUTIONTIME$\t$SERVICELATENCY$\t$SERVICEOUTPUT$\t$SERVICEPERFDATA$" << /var/log/centreon-engine/service-perfdata.dat
  }

.. note::

   This method, while flexible, comes with a relatively high CPU
   overhead. If you're processing performance data for a large number of
   hosts and services, you'll probably want Centreon Engine to write
   performance data to files instead. This method is described in the
   next section.

Writing Performance Data To Files
=================================

You can have Centreon Engine write all host and service performance data
directly to text files using the :ref:`host_perfdata_file <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariableshostperformance_datafile>`
and :ref:`service_perfdata_file <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariablesserviceperformance_datafile>`
options. The format in which host and service performance data is
written to those files is determined by the
:ref:`template host_perfdata_file_template <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariableshostperformance_datafile>`
and :ref:`service_perfdata_file_template <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariablesserviceperformance_datafiletemplate>`
options.

An example file format template for service performance data might look
like this::

  service_perfdata_file_template=[SERVICEPERFDATA]\t$TIMET$\t$HOSTNAME$\t$SERVICEDESC$\t$SERVICEEXECUTIONTIME$\t$SERVICELATENCY$\t$SERVICEOUTPUT$\t$SERVICEPERFDATA$

By default, the text files will be opened in "append" mode. If you need
to change the modes to "write" or "non-blocking read/write" (useful when
writing to pipes), you can use the
:ref:`host_perfdata_file_mode <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariableshostperformance_datafilemode>`
and :ref:`service_perfdata_file_mode <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariablesserviceperformance_datafilemode>`
options.

Additionally, you can have Centreon Engine periodically execute commands
to periocially process the performance data files (e.g. rotate them)
using the :ref:`host_perfdata_file_processing_command <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariableshostperformance_datafileprocessingcommand>`
and :ref:`service_perfdata_file_processing_command <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariablesserviceperformance_datafileprocessingcommand>`
options. The interval at which these commands are executed are governed
by the :ref:`host_perfdata_file_processing_interval <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariableshostperformance_datafileprocessinginterval>`
and :ref:`service_perfdata_file_processing_interval <basics_main_configuration_file_options#main_configuration_file_optionsconfigurationfilevariablesserviceperformance_datafileprocessinginterval>`
options, respectively.
