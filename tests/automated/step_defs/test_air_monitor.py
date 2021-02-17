# coding=utf-8
"""Air Monitor feature tests."""

from pytest_bdd import (
    given,
    scenario,
    then,
    when,
)


@scenario('features/Air-monitor.feature', 'Metric harvesting')
def test_metric_harvesting():
    """Metric harvesting."""


@scenario('features/Air-monitor.feature', 'The Yandex Monitoring presents metrics')
def test_the_yandex_monitoring_presents_metrics():
    """The Yandex Monitoring presents metrics."""


@scenario('features/Air-monitor.feature', 'The Yandex sends alerts when air quality is poor')
def test_the_yandex_sends_alerts_when_air_quality_is_poor():
    """The Yandex sends alerts when air quality is poor."""


@given('A Device has harvested metrics')
def a_device_has_harvested_metrics():
    """A Device has harvested metrics."""
    raise NotImplementedError


@given('Alice ability is activated')
def alice_ability_is_activated():
    """Alice ability is activated."""
    raise NotImplementedError


@given('Metrics are stored')
def metrics_are_storedand_alert_are_configured_for_metrics():
    """Metrics are stored
And Alert are configured for metrics."""
    raise NotImplementedError

@given('Alert are configured for metrics')
def metrics_configured_for_metrics():
    """Metrics are stored"""
    raise NotImplementedError

@given('Metrics are stored')
def metrics_are_stored():
    """Metrics are stored"""
    raise NotImplementedError

@given('the Yandex Monitoring dashboard is opened')
def metrics_yandex_monitoring_dashboard_is_opened():
    """And the Yandex Monitoring dashboard is opened."""
    raise NotImplementedError

@when('An User is looking for the air quality')
def an_user_is_looking_for_the_air_quality():
    """An User is looking for the air quality."""
    raise NotImplementedError


@when('Metrics run into alerting values')
def metrics_run_into_alerting_values():
    """Metrics run into alerting values."""
    raise NotImplementedError


@when('The Device flush the metrics')
def the_device_flush_the_metrics():
    """The Device flush the metrics."""
    raise NotImplementedError


@when('an User say the light control commands')
def an_user_say_the_light_control_commands():
    """an User say the light control commands."""
    raise NotImplementedError

@then('The Metrics are stored')
def the_metrics_are_stored():
    """The Metrics are stored."""
    raise NotImplementedError
